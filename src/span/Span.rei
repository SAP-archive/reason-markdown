let identify: list(string) => list(SpanTypes.span);

let get_ast: (list(SpanTypes.span), BlockContext.t) => AST.spans;

let get_markdown: (AST.spans, BlockContext.t) => string;
