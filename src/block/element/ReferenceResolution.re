let get_start: list(string) => option(BlockTypes.reference_resolution) =
  lines =>
    if (Js.Re.test_(
          Js.Re.fromString("^ {0,4}\\[.*\\] *:.*"),
          List.hd(lines),
        )) {
      let referenceMatch =
        Js.String.match(
          Js.Re.fromString(
            "^ *\\[(([^\\\\[\\]!]|\\\\.|![^\\[])*((!\\[([^\\\\\\[\\]]|\\\\.)*\\](\\[([^\\\\\\[\\]]|\\\\.)*\\])?)?([^\\\\\\[\\]]|\\\\.)*)*)\\] *:(.*)",
          ),
          List.hd(lines),
        );

      switch (referenceMatch) {
      | None => None
      | Some(referenceResult) =>
        let referenceValueMatch =
          Js.String.match(
            Js.Re.fromString("^ *([^ <>]+|<[^<>]*>)( .*)?"),
            referenceResult[9],
          );

        switch (referenceValueMatch) {
        | None => None
        | Some(referenceValueResult) =>
          Some({
            unprocessedReferenceIdString: referenceResult[1],
            unprocessedUrlString: referenceValueResult[1],
            refDefinitionTrailingSequence: referenceValueResult[2],
          })
        };
      };
    } else {
      None;
    };

let getLineCount: (BlockTypes.reference_resolution, list(string)) => int =
  (block, lines) =>
    if (Js.Re.test_(
          Js.Re.fromString({js|^[^\u0020]|js}),
          block.refDefinitionTrailingSequence,
        )
        && List.length(lines) > 1
        && Js.Re.test_(
             Js.Re.fromString(
               "^ +(\"(([^\"\\]|\\.)*)\"|\'(([^'\\]|\\.)*)\'|\\(([^\\()]|\\.)*\\)) *",
             ),
             List.nth(lines, 1),
           )) {
      2;
    } else {
      1;
    };

let try_parse: list(string) => option(BlockTypes.t) =
  source =>
    switch (get_start(source)) {
    | None => None
    | Some(data) =>
      let lineCount = getLineCount(data, source);

      Some({
        blockType: BlockTypes.ReferenceResolution(data),
        lines: Util.getNFirst(source, lineCount),
      });
    };

let interprete: BlockTypes.t => option(BlockContext.reference) =
  source =>
    switch (source.blockType) {
    | ReferenceResolution(data) =>
      let reference_id = Util.simplify(data.unprocessedReferenceIdString);
      let link_url = Util.process_url(data.unprocessedUrlString);
      let title_container: ref(string) = ref("");
      let link_title: ref(string) = ref("");

      if (List.length(source.lines) > 1) {
        title_container := List.nth(source.lines, 1);
      } else {
        title_container := data.refDefinitionTrailingSequence;
      };

      let title_container_regex =
        Js.Re.fromString("^\\((([^\\\\()]|\\\\.)*)\\)");
      let matches_title_container =
        Js.String.match(title_container_regex, title_container^);

      switch (matches_title_container) {
      | None =>
        let quoted_title_regex = Js.Re.fromString("^\"([^\"]*)\"");
        let matches_quoted_title =
          Js.String.match(quoted_title_regex, title_container^);

        switch (matches_quoted_title) {
        | None => ()
        | Some(captures) => link_title := captures[1]
        };
      | Some(captures) => link_title := captures[1]
      };

      Some((reference_id, link_url, link_title^));
    | _ => None
    };

let get_ast: (BlockTypes.reference_resolution, list(string)) => AST.block =
  (data, lines) => {
    let reference_id = Util.simplify(data.unprocessedReferenceIdString);
    let link_url = Util.process_url(data.unprocessedUrlString);
    let title_container: ref(string) = ref("");
    let link_title: ref(string) = ref("");

    if (List.length(lines) > 1) {
      title_container := List.nth(lines, 1);
    } else {
      title_container := data.refDefinitionTrailingSequence;
    };

    let title_container_regex =
      Js.Re.fromString("^\\((([^\\\\()]|\\\\.)*)\\)");
    let matches_title_container =
      Js.String.match(title_container_regex, title_container^);

    switch (matches_title_container) {
    | None =>
      let quoted_title_regex = Js.Re.fromString("^\"([^\"]*)\"");
      let matches_quoted_title =
        Js.String.match(quoted_title_regex, title_container^);

      switch (matches_quoted_title) {
      | None => ()
      | Some(captures) => link_title := captures[1]
      };
    | Some(captures) => link_title := captures[1]
    };

    ReferenceResolution({
      referenceId: reference_id,
      linkUrl: link_url,
      linkTitle: link_title^,
    });
  };

let get_markdown: AST.reference_resolution_data => list(string) =
  data => [
    "[" ++ data.referenceId ++ "]:" ++ data.linkUrl ++ " " ++ data.linkTitle,
  ];
