let beginsWith4Spaces: string => bool =
  source => Js.Re.test_(Js.Re.fromString({js|^ {4,}|js}), source);

let rec readTillEndLine: list(string) => list(string) =
  source =>
    if (! Util.isBlankLine(List.hd(source))
        && ! beginsWith4Spaces(List.hd(source))) {
      [];
    } else if (List.length(source) > 1
               && Util.isBlankLine(List.hd(source))
               && ! beginsWith4Spaces(List.nth(source, 1))) {
      [];
    } else {
      switch (source) {
      | [element] => [element]
      | [head, ...tail] => [head, ...readTillEndLine(tail)]
      | c => c
      };
    };

let get_start: list(string) => option(BlockTypes.code) =
  source =>
    if (beginsWith4Spaces(List.hd(source))) {
      Some({lines: readTillEndLine(source)});
    } else {
      None;
    };

let getLineCount: (BlockTypes.code, list(string)) => int =
  (block, _lines) => List.length(block.lines);

let try_parse: list(string) => option(BlockTypes.t) =
  source =>
    switch (get_start(source)) {
    | None => None
    | Some(data) =>
      Some({
        blockType: BlockTypes.Code(data),
        lines: Util.getNFirst(source, getLineCount(data, source)),
      })
    };

let get_ast: BlockTypes.code => AST.block =
  data =>
    AST.Code(
      String.concat(
        "\n",
        List.map(
          line => Js.String.replaceByRe(Js.Re.fromString("^ {4}"), "", line),
          data.lines,
        ),
      ),
    );
