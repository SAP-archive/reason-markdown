let get_start: list(string) => option(BlockTypes.atx_style_header) =
  source =>
    if (Js.Re.test_(Js.Re.fromString({js|^#|js}), List.hd(source))) {
      Some({line: List.hd(source)});
    } else {
      None;
    };

let try_parse: list(string) => option(BlockTypes.t) =
  source =>
    switch (get_start(source)) {
    | None => None
    | Some(data) =>
      Some({
        blockType: BlockTypes.AtxStyleHeader(data),
        lines: Util.getNFirst(source, 1),
      })
    };

let get_ast: (BlockTypes.atx_style_header, BlockContext.t) => AST.block =
  (source, context) => {
    let regex = Js.Re.fromString("^(#+) (.*[^#])#*$");

    switch (Js.String.match(regex, source.line)) {
    | None => Null
    | Some(captures) =>
      let level = String.length(captures[1]);
      let inline_ast = Span.get_ast(Span.identify([captures[2]]), context);

      Heading(level, inline_ast);
    };
  };

let get_markdown: (int, AST.spans, BlockContext.t) => list(string) =
  (level, spans, context) => {
    let output: ref(string) = ref("");

    for (_count in 1 to level) {
      output := output^ ++ "#";
    };

    output := output^ ++ " " ++ Span.get_markdown(spans, context);

    [output^];
  };
