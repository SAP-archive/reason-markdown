let is_start_line: string => bool =
  source => Js.Re.test(source, Js.Re.fromString("^( *[\\-\\*\\+] +)[^ ]"));

let get_starter_pattern: string => string =
  source => {
    let regex = Js.Re.fromString({js|^( *[-*+] +)[^ ]|js});

    switch (Js.String.match(regex, source)) {
    | None => ""
    | Some(captures) => captures[1]
    };
  };

let escape_starter_pattern: string => string =
  source => {
    let escaped_pattern = ref(source);

    escaped_pattern :=
      Js.String.replaceByRe(
        Js.Re.fromStringWithFlags("\\+", "g"),
        "\\+",
        escaped_pattern^,
      );
    escaped_pattern :=
      Js.String.replaceByRe(
        Js.Re.fromStringWithFlags("\\*", "g"),
        "\\*",
        escaped_pattern^,
      );

    escaped_pattern^;
  };

let rec read_till_end_line: (list(string), string) => list(string) =
  (lines, starterPattern) =>
    if (List.length(lines) > 1
        && Util.isBlankLine(List.nth(lines, 0))
        && Util.isBlankLine(List.nth(lines, 1))) {
      [];
    } else if (List.length(lines) > 1
               && Util.isBlankLine(List.nth(lines, 0))
               && !
                    Js.Re.test(
                      List.nth(lines, 1),
                      Js.Re.fromString(
                        "^" ++ escape_starter_pattern(starterPattern),
                      ),
                    )
               && !
                    Js.Re.test(
                      List.nth(lines, 1),
                      Js.Re.fromString(
                        {js|^\u0020{|js}
                        ++ string_of_int(String.length(starterPattern))
                        ++ {js|,}[^ \u0020]|js},
                      ),
                    )) {
      [];
    } else if (List.length(lines) > 1
               && ! Util.isBlankLine(List.nth(lines, 0))
               && !
                    Js.Re.test(
                      List.nth(lines, 1),
                      Js.Re.fromString(
                        "^" ++ escape_starter_pattern(starterPattern),
                      ),
                    )
               && !
                    Js.Re.test(
                      List.nth(lines, 1),
                      Js.Re.fromString(
                        {js|^\u0020{|js}
                        ++ string_of_int(String.length(starterPattern))
                        ++ {js|,}[^ \u0020]|js},
                      ),
                    )
               && !
                    Js.Re.test(
                      List.nth(lines, 1),
                      Js.Re.fromString({js|^\u0020{4,}|js}),
                    )
               && (
                 Js.Re.test(
                   List.nth(lines, 1),
                   Js.Re.fromString("^( *[*\\-+] +)[^ ]"),
                 )
                 || Js.Re.test(
                      List.nth(lines, 1),
                      Js.Re.fromString({js|^( *([0-9]+). +)[^ ]|js}),
                    )
                 || Js.Re.test(
                      List.nth(lines, 1),
                      Js.Re.fromString(
                        "^ *((\\* *\\* *\\* *[* ]*)|(- *- *- *[- ]*)|(_ *_ *_ *[_ ]*))$",
                      ),
                    )
               )) {
      [List.hd(lines)];
    } else {
      switch (lines) {
      | [el1] => [el1]
      | [head, ...tail] => [
          head,
          ...read_till_end_line(tail, starterPattern),
        ]
      | c => c
      };
    };

let get_start: list(string) => option(BlockTypes.unordered_list) =
  source =>
    if (is_start_line(List.hd(source))) {
      let starterPattern = get_starter_pattern(List.hd(source));
      Some({
        starterPattern,
        lines: [
          List.hd(source),
          ...read_till_end_line(List.tl(source), starterPattern),
        ],
      });
    } else {
      None;
    };

let getLineCount: (BlockTypes.unordered_list, list(string)) => int =
  (block, _lines) => List.length(block.lines);

let try_parse: list(string) => option(BlockTypes.t) =
  source =>
    switch (get_start(source)) {
    | None => None
    | Some(data) =>
      Some({
        blockType: BlockTypes.UnorderedList(data),
        lines: Util.getNFirst(source, getLineCount(data, source)),
      })
    };

let get_list_items:
  BlockTypes.unordered_list => list(BlockTypes.unordered_list_item) =
  source => {
    let current_position: ref(int) = ref(0);
    let list_items: ref(list(BlockTypes.unordered_list_item)) = ref([]);
    let current_list_item: ref(BlockTypes.unordered_list_item) = ref([]);
    let found_start: ref(bool) = ref(false);

    let starter_pattern_regex =
      Js.Re.fromString("^" ++ escape_starter_pattern(source.starterPattern));

    while (current_position^ < List.length(source.lines)) {
      let current_line = List.nth(source.lines, current_position^);

      switch (Js.String.match(starter_pattern_regex, current_line)) {
      | None =>
        let leading_space_regex = Js.Re.fromString("^(\\s+)");

        switch (Js.String.match(leading_space_regex, current_line)) {
        | None =>
          current_list_item :=
            List.append(current_list_item^, [current_line])
        | Some(captures) =>
          let leading_whitespace_length = String.length(captures[1]);

          if (leading_whitespace_length
              <= String.length(source.starterPattern)) {
            current_list_item :=
              List.append(
                current_list_item^,
                [
                  Js.String.replaceByRe(leading_space_regex, "", current_line),
                ],
              );
          } else {
            current_list_item :=
              List.append(
                current_list_item^,
                [
                  String.sub(
                    current_line,
                    String.length(source.starterPattern),
                    String.length(current_line)
                    - String.length(source.starterPattern),
                  ),
                ],
              );
          };
        };
      | Some(captures) =>
        if (found_start^) {
          list_items := List.append(list_items^, [current_list_item^]);
        } else {
          found_start := true;
        };
        current_list_item :=
          [
            String.sub(
              current_line,
              String.length(source.starterPattern),
              String.length(current_line)
              - String.length(source.starterPattern),
            ),
          ];
      };

      current_position := current_position^ + 1;
    };

    list_items := List.append(list_items^, [current_list_item^]);

    list_items^;
  };
