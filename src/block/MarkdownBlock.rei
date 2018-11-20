let identify_blocks: list(string) => (list(BlockTypes.t), BlockContext.t);

let get_ast: (list(BlockTypes.t), BlockContext.t) => AST.t;

let get_context: AST.t => list(BlockContext.reference);

let get_markdown: (AST.t, BlockContext.t, bool) => list(string);
