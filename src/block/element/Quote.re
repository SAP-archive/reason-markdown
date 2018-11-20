let is_start_line: string => bool =
  source => {
    let regex = Js.Re.fromString({js|^\u0020*>|js});
    Js.Re.test(source, regex);
  };

let rec read_till_end_line: list(string) => list(string) =
  source =>
    if (List.length(source) < 2) {
      source;
    } else if (Util.isBlankLine(List.nth(source, 0))
        && Util.isBlankLine(List.nth(source, 1))) {
      [];
    } else if (Util.isBlankLine(List.nth(source, 0))
               && Js.Re.test(
                    List.nth(source, 1),
                    Js.Re.fromString({js|^\u0020{4,}|js}),
                  )) {
      [];
    } else if (Util.isBlankLine(List.nth(source, 0))
               && Js.Re.test(
                    List.nth(source, 1),
                    Js.Re.fromString({js|^\u0020*[^>]|js}),
                  )) {
      [];
    } else if (Util.isBlankLine(List.nth(source, 0))
               && Js.Re.test(
                    List.nth(source, 1),
                    Js.Re.fromString(
                      "^ {0,3}((\\* *\\* *\\* *[* ]*)|(- *- *- *[- ]*)|(_ *_ *_ *[_ ]*))",
                    ),
                  )) {
      [];
    } else {
      switch (source) {
      | [el1, el2] => [el1, el2]
      | [head, ...tail] => [head, ...read_till_end_line(tail)]
      | c => c
      };
    };

let get_start: list(string) => option(BlockTypes.quote) =
  source =>
    if (is_start_line(List.hd(source))) {
      Some({lines: read_till_end_line(source)});
    } else {
      None;
    };

let getLineCount: (BlockTypes.quote, list(string)) => int =
  (block, _lines) => List.length(block.lines);

let try_parse: list(string) => option(BlockTypes.t) =
  source =>
    switch (get_start(source)) {
    | None => 
      None
    | Some(data) =>
      Some({
        blockType: BlockTypes.Quote(data),
        lines: Util.getNFirst(source, getLineCount(data, source)),
      })
    };

let remove_quote_pattern: list(string) => list(string) =
  source => {
    let regex = Js.Re.fromString("^ *>");

    List.map(
      line =>
        Js.String.replaceByRe(regex, "", line),
      source,
    );
  };

let get_inner_blocks: BlockTypes.quote => list(BlockTypes.t) =
  source => {
    let blocks: ref(list(BlockTypes.t)) = ref([]);
    let remainingLines: ref(list(string)) =
      ref(remove_quote_pattern(source.lines));

    while (List.length(remainingLines^) > 0) {
      let matchedLines = ref(0);
      let matched = ref(false);

      if (! matched^) {
        if (Util.isBlankLine(List.hd(remainingLines^))) {
          blocks := List.append(blocks^, [{
            blockType: Null,
            lines: [List.hd(remainingLines^)]
          }]);
          matchedLines := 1;
          matched := true;
        };
      };

      if (! matched^) {
        switch (CodeBlock.try_parse(remainingLines^)) {
        | None => ()
        | Some(context) =>
          blocks := List.append(blocks^, [context]);
          matchedLines := List.length(context.lines);
          matched := true;
        };
      };

      if (! matched^) {
        let context = Paragraph.parse(remainingLines^);

        blocks := List.append(blocks^, [context]);
        matchedLines := List.length(context.lines);
        matched := true;
      };

      remainingLines := Util.removeNFirst(remainingLines^, matchedLines^);
    };

    blocks^;
  };
