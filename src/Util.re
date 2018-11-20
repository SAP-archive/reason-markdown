let trim: string => string = source => String.trim(source);

let simplify: string => string =
  source =>
    Js.String.replaceByRe(
      Js.Re.fromStringWithFlags(
        {js|[\u0009\u000A\u000C\u000D\u0020]+|js},
        "g",
      ),
      {js|\u0020|js},
      trim(source),
    );

let simplifyLineBreaks: string => string =
  source =>
    Js.String.replaceByRe(
      Js.Re.fromStringWithFlags({js|\u000D\u000A|js}, "g"),
      {js|\u000A|js},
      source,
    );

let space = {js|\u0020|js};

let tab = {js|\u0009|js};

let lineBreak = {js|\u000A|js};

let whitespace = {js|(?:\u0009|\u000A|\u000C|\u000D|\u0020)|js};

type lineType =
  | Line(string)
  | BlankLine;

let initLines: string => list(string) =
  source => {
    let lines: ref(list(string)) = ref([]);
    let doc: ref(string) = ref(source);
    let regex = Js.Re.fromString({js|^([^\u000A]*)\u000A|js});

    while (String.length(doc^) > 0) {
      if (Js.String.search(Js.Re.fromString({js|\u000A|js}), doc^) == (-1)) {
        lines := List.append(lines^, [doc^]);
        doc := "";
      } else {
        switch (Js.String.match(regex, doc^)) {
        | None => ()
        | Some(captures) =>
          lines := List.append(lines^, [captures[1]]);
          doc := Js.String.replace(captures[0], "", doc^);
        };
      };
    };

    lines^;
  };

let expandTabs: string => string =
  source => {
    let result: ref(string) = ref(source);
    let p: ref(int) =
      ref(Js.String.search(Js.Re.fromString({js|\u0009|js}), result^));

    while (p^ > (-1)) {
      result :=
        Js.String.replaceByRe(
          Js.Re.fromString({js|\u0009|js}),
          Js.String.repeat(4 - p^ mod 4, {js|\u0020|js}),
          result^,
        );

      p := Js.String.search(Js.Re.fromString({js|\u0009|js}), result^);
    };

    result^;
  };

let prepare: string => list(string) =
  source => {
    let lines = initLines(source);

    List.map(line => expandTabs(line), lines);
  };

let isBlankLine: string => bool =
  source =>
    Js.String.match(
      Js.Re.fromString({js|^[\u0020\u0009\u000A\u000C\u000D]*$|js}),
      source,
    )
    != None;

let rec getNFirst: (list('a), int) => list('a) =
  (source, n) =>
    if (n == 0) {
      [];
    } else {
      switch (source) {
      | [head, ...tail] => [head, ...getNFirst(tail, n - 1)]
      | [] => []
      };
    };

let rec removeNFirst: (list('a), int) => list('a) =
  (source, n) =>
    if (n == 0) {
      source;
    } else {
      switch (source) {
      | [head, ...tail] => removeNFirst(tail, n - 1)
      | [] => []
      };
    };

let process_url: string => string =
  source => {
    let find_regex = Js.Re.fromStringWithFlags("[\\s<>]", "g");

    Js.String.replaceByRe(find_regex, "", source);
  };

let rec concat_text: (AST.spans, string) => AST.spans = (remaining, text) => {
  switch (remaining) {
  | [head, ...tail] =>
    switch (head) {
    | Text(following_text) =>
      concat_text(tail, String.concat("", [text, following_text]));
    | span =>
      if (String.length(text) > 0) {
        [Text(text), span, ...concat_text(tail, "")]
      } else {
        [span, ...concat_text(tail, "")]
      }
    }
  | [] =>
    if (String.length(text) > 0) {
      [Text(text)]
    } else {
      []
    }
  }
};

let rec simplify_spans_intern: (AST.spans, bool, bool) => AST.spans = (spans, in_emph, in_strong) => {
  let current_position: ref(int) = ref(0);
  let new_spans: ref(AST.spans) = ref([]);

  while (current_position^ < List.length(spans)) {
    let span = List.nth(spans, current_position^);

    switch (span) {
    | EmphaticStress(pattern, content) =>
      if (in_emph) {
        new_spans := List.append(new_spans^, List.append([AST.Text(pattern), ...simplify_spans_intern(content, true, in_strong)], [AST.Text(pattern)]))
      } else if (List.length(content) == 1 && ! in_strong) {
          switch (List.hd(content)) {
          | StrongImportance(strong_pattern, strong_content) =>
            new_spans := List.append(new_spans^, [AST.EmphaticStressStrongImportance(pattern ++ strong_pattern, simplify_spans_intern(strong_content, true, true))]);
          | x => new_spans := List.append(new_spans^, [AST.EmphaticStress(pattern, simplify_spans_intern(content, true, in_strong))])
          }
      } else {
        new_spans := List.append(new_spans^, [AST.EmphaticStress(pattern, simplify_spans_intern(content, true, in_strong))]);
      };
    | StrongImportance(pattern, content) =>
      if (in_strong) {
        new_spans := List.append(new_spans^, List.append([AST.Text(pattern), ...simplify_spans_intern(content, in_emph, true)], [AST.Text(pattern)]))
      } else if (List.length(content) == 1 && ! in_emph) {
          switch (List.hd(content)) {
          | EmphaticStress(emph_pattern, emph_content) =>
          new_spans := List.append(new_spans^, [AST.EmphaticStressStrongImportance(pattern ++ emph_pattern, simplify_spans_intern(emph_content, true, true))]);
          }
      } else {
        new_spans := List.append(new_spans^, [AST.StrongImportance(pattern, simplify_spans_intern(content, in_emph, true))]);
      };
    | EmphaticStressStrongImportance(pattern, content) =>
      if (in_emph && in_strong) {
        new_spans := List.append(new_spans^, List.append([AST.Text(pattern), ...simplify_spans_intern(content, true, true)], [AST.Text(pattern)]))
      } else if (in_emph && ! in_strong) {
        new_spans := List.append(new_spans^, List.append(new_spans^, [AST.StrongImportance(String.sub(pattern, 0, 2), simplify_spans_intern(content, true, true))]));
      } else if (in_strong && ! in_emph) {
        new_spans := List.append(new_spans^, List.append(new_spans^, [AST.EmphaticStress(String.sub(pattern, 0, 2), simplify_spans_intern(content, true, true))]));
      } else {
        new_spans := List.append(new_spans^, List.append(new_spans^, [AST.EmphaticStressStrongImportance(pattern, simplify_spans_intern(content, true, true))]));
      };
    | c => new_spans := List.append(new_spans^, [c]);
    };

    current_position := current_position^ + 1;
  };

  concat_text(new_spans^, "");
};

let rec simplify_spans: AST.spans => AST.spans = spans =>
  simplify_spans_intern(spans, false, false);

let rec simplify_ast: AST.t => AST.t =
  ast => {
    List.map((block: AST.block) => {
      switch (block) {
        | Heading(level, spans) => AST.Heading(level, simplify_spans(spans))
        | Paragraph(spans) => AST.Paragraph(simplify_spans(spans))
        | Quote(blocks) => AST.Quote(simplify_ast(blocks))
        | UnorderedList(items) =>
          AST.UnorderedList(List.map(item => simplify_ast(item), items))
        | OrderedList(items) =>
          AST.OrderedList(
            List.map(
              (item: AST.ordered_list_item) => {
                AST.number: item.number,
                AST.blocks: simplify_ast(item.blocks),
              },
              items,
            ),
          )
        | block => block
        };
    }, ast);
  };
