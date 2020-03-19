let markdown_to_ast: string => AST.t =
  markdown => {
    let lines = Util.prepare(markdown);
    let (blocks, context) = MarkdownBlock.identify_blocks(lines);

    MarkdownBlock.get_ast(blocks, context);
  };

let ast_to_markdown: AST.t => string =
  ast => {
    let context: BlockContext.t = {
      references: MarkdownBlock.get_context(ast),
    };

    let lines: list(string) =
      MarkdownBlock.get_markdown(ast, context, false);

    String.concat("\n", lines);
  };
