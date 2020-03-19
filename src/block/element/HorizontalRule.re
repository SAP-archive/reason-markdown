let get_start: list(string) => option(BlockTypes.horizontal_rule) =
  source =>
    if (Js.Re.test_(
          Js.Re.fromString(
            "^ *((\\* *\\* *\\* *[* ]*)|(- *- *- *[- ]*)|(_ *_ *_ *[_ ]*))$",
          ),
          List.hd(source),
        )) {
      Some();
    } else {
      None;
    };

let try_parse: list(string) => option(BlockTypes.t) =
  source =>
    switch (get_start(source)) {
    | None => None
    | Some(data) =>
      Some({
        blockType: BlockTypes.HorizontalRule(data),
        lines: Util.getNFirst(source, 1),
      })
    };
