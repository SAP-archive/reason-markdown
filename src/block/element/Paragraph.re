let rec read_till_end: list(string) => list(string) =
  source =>
    if (Util.isBlankLine(List.hd(source))) {
      [];
    } else if (List.length(source) > 1
               && !
                    Js.Re.test_(
                      Js.Re.fromString({js|^\u0020{4,}|js}),
                      List.nth(source, 1),
                    )
               && (
                 Js.Re.test_(
                   Js.Re.fromString(
                     "^ *((\\* *\\* *\\* *[* ]*)|(- *- *- *[- ]*)|(_ *_ *_ *[_ ]*))$",
                   ),
                   List.nth(source, 1),
                 )
                 || Js.Re.test_(
                      Js.Re.fromString({js|^\u0020*>|js}),
                      List.nth(source, 1),
                    )
                 || Js.Re.test_(
                      Js.Re.fromString({js|^( *([0-9]+). +)[^ ]|js}),
                      List.nth(source, 1),
                    )
                 || Js.Re.test_(
                      Js.Re.fromString("^( *[\\-\\*\\+] +)[^ ]"),
                      List.nth(source, 1),
                    )
               )) {
      [List.nth(source, 0)];
    } else {
      switch (source) {
      | [el1] => [el1]
      | [head, ...tail] => [head, ...read_till_end(tail)]
      | c => c
      };
    };

let prepare_line: string => string =
  source =>
    Js.String.replaceByRe(
      Js.Re.fromStringWithFlags({js|\\<|js}, ~flags="g"),
      "&lt;",
      source,
    );

let get_start: list(string) => BlockTypes.paragraph =
  source => {lines: read_till_end(source)};

let getLineCount: (BlockTypes.paragraph, list(string)) => int =
  (block, _lines) => List.length(block.lines);

let parse: list(string) => BlockTypes.t =
  source => {
    let data = get_start(source);

    {
      blockType: BlockTypes.Paragraph(data),
      lines: Util.getNFirst(source, getLineCount(data, source)),
    };
  };

let get_ast: (BlockTypes.paragraph, BlockContext.t) => AST.block =
  (source, context) => {
    let inline_ast = Span.get_ast(Span.identify(source.lines), context);

    Paragraph(inline_ast);
  };

let get_markdown: (AST.spans, BlockContext.t) => list(string) =
  (spans, context) => {
    let output_string = Span.get_markdown(spans, context);

    Util.initLines(output_string);
  };
