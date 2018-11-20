let get_start: list(string) => option(BlockTypes.setext_style_header) =
  source =>
    if (List.length(source) > 1
        && Js.Re.test(
             List.nth(source, 1),
             Js.Re.fromString({js|^(-+|=+) *$|js}),
           )) {
      Some({text: List.hd(source), secondLine: List.nth(source, 1)});
    } else {
      None;
    };

let try_parse: list(string) => option(BlockTypes.t) =
  source =>
    switch (get_start(source)) {
    | None => None
    | Some(data) =>
      Some({
        blockType: BlockTypes.SetextStyleHeader(data),
        lines: Util.getNFirst(source, 2),
      })
    };

let get_ast: (BlockTypes.setext_style_header, BlockContext.t) => AST.block =
  (source, context) => {
    let inline_ast = Span.get_ast(Span.identify([source.text]), context);

    switch (source.secondLine.[0]) {
    | '-' => Heading(2, inline_ast)
    | '=' => Heading(1, inline_ast)
    | _ => Null
    };
  };
