let identify_blocks: list(string) => (list(BlockTypes.t), BlockContext.t) =
  source => {
    let blocks: ref(list(BlockTypes.t)) = ref([]);
    let remainingLines: ref(list(string)) = ref(source);
    let context: ref(BlockContext.t) = ref({BlockContext.references: []});

    while (List.length(remainingLines^) > 0) {
      let first = List.hd(remainingLines^);
      let matchedLines = ref(0);
      let matched = ref(false);

      if (Util.isBlankLine(first)) {
        blocks :=
          List.append(
            blocks^,
            [{blockType: Null, lines: [List.hd(remainingLines^)]}],
          );
        matchedLines := 1;
        matched := true;
      };

      /* Check if any other rule matched before, if not try to parse ReferenceResolution */
      if (! matched^) {
        switch (ReferenceResolution.try_parse(remainingLines^)) {
        | None => ()
        | Some(block) =>
          blocks := List.append(blocks^, [block]);
          switch (ReferenceResolution.interprete(block)) {
          | Some(data) =>
            context :=
              {
                ...context^,
                references: List.append(context^.references, [data]),
              }
          | None => ()
          };

          matchedLines := List.length(block.lines);
          matched := true;
        };
        ();
      };

      /* Check if any other rule matched before, if not try to parse SetextStyleHeader */
      if (! matched^) {
        switch (SetextStyleHeader.try_parse(remainingLines^)) {
        | None => ()
        | Some(context) =>
          blocks := List.append(blocks^, [context]);
          matchedLines := List.length(context.lines);
          matched := true;
        };
      };

      /* Check if any other rule matched before, if not try to parse CodeBlock */
      if (! matched^) {
        switch (CodeBlock.try_parse(remainingLines^)) {
        | None => ()
        | Some(context) =>
          blocks := List.append(blocks^, [context]);
          matchedLines := List.length(context.lines);
          matched := true;
        };
      };

      /* Check if any other rule matched before, if not try to parse AtxStyleHeader */
      if (! matched^) {
        switch (AtxStyleHeader.try_parse(remainingLines^)) {
        | None => ()
        | Some(context) =>
          blocks := List.append(blocks^, [context]);
          matchedLines := List.length(context.lines);
          matched := true;
        };
      };

      /* Check if any other rule matched before, if not try to parse Quote */
      if (! matched^) {
        switch (Quote.try_parse(remainingLines^)) {
        | None => ()
        | Some(context) =>
          blocks := List.append(blocks^, [context]);
          matchedLines := List.length(context.lines);
          matched := true;
        };
      };

      /* Check if any other rule matched before, if not try to parse HorizontalRule */
      if (! matched^) {
        switch (HorizontalRule.try_parse(remainingLines^)) {
        | None => ()
        | Some(context) =>
          blocks := List.append(blocks^, [context]);
          matchedLines := List.length(context.lines);
          matched := true;
        };
      };

      /* Check if any other rule matched before, if not try to parse UnorderedList */
      if (! matched^) {
        switch (UnorderedList.try_parse(remainingLines^)) {
        | None => ()
        | Some(context) =>
          blocks := List.append(blocks^, [context]);
          matchedLines := List.length(context.lines);
          matched := true;
        };
      };

      /* Check if any other rule matched before, if not try to parse OrderedList */
      if (! matched^) {
        switch (OrderedList.try_parse(remainingLines^)) {
        | None => ()
        | Some(context) =>
          blocks := List.append(blocks^, [context]);
          matchedLines := List.length(context.lines);
          matched := true;
        };
      };

      /* Check if any other rule matched before, if not parse a Paragraph */
      if (! matched^) {
        let context = Paragraph.parse(remainingLines^);

        blocks := List.append(blocks^, [context]);
        matchedLines := List.length(context.lines);
        matched := true;
      };

      remainingLines := Util.removeNFirst(remainingLines^, matchedLines^);
    };

    (blocks^, context^);
  };

let rec get_ast: (list(BlockTypes.t), BlockContext.t) => AST.t =
  (blocks, context) => {
    let current_context: ref(BlockContext.t) = ref(context);

    let ast: ref(AST.t) = ref([]);
    let current_position: ref(int) = ref(0);

    while (current_position^ < List.length(blocks)) {
      let current_block = List.nth(blocks, current_position^);

      switch (current_block.blockType) {
      | Null => ()
      | HorizontalRule(_) => ast := List.append(ast^, [HorizontalRule])
      | Code(data) => ast := List.append(ast^, [CodeBlock.get_ast(data)])
      | AtxStyleHeader(data) =>
        ast :=
          List.append(
            ast^,
            [AtxStyleHeader.get_ast(data, current_context^)],
          )
      | SetextStyleHeader(data) =>
        ast :=
          List.append(
            ast^,
            [SetextStyleHeader.get_ast(data, current_context^)],
          )
      | ReferenceResolution(data) =>
        ast :=
          List.append(
            ast^,
            [ReferenceResolution.get_ast(data, current_block.lines)],
          )
      | Quote(data) =>
        let inner_blocks = Quote.get_inner_blocks(data);
        ast :=
          List.append(
            ast^,
            [Quote(get_ast(inner_blocks, current_context^))],
          );
      | Paragraph(data) =>
        ast :=
          List.append(ast^, [Paragraph.get_ast(data, current_context^)])
      | OrderedList(data) =>
        let items = OrderedList.get_list_items(data);
        let items_ast: list(AST.ordered_list_item) =
          List.map(
            (item: BlockTypes.ordered_list_item) => {
              let (inner_blocks, context) = identify_blocks(item.lines);

              current_context :=
                {
                  ...current_context^,
                  references:
                    List.append(
                      current_context^.references,
                      context.references,
                    ),
                };

              {
                AST.number: item.number,
                AST.blocks: get_ast(inner_blocks, current_context^),
              };
            },
            items,
          );
        ast := List.append(ast^, [OrderedList(items_ast)]);
      | UnorderedList(data) =>
        let items = UnorderedList.get_list_items(data);
        let items_ast: list(AST.unordered_list_item) =
          List.map(
            item => {
              let (inner_blocks, context) = identify_blocks(item);

              current_context :=
                {
                  ...current_context^,
                  references:
                    List.append(
                      current_context^.references,
                      context.references,
                    ),
                };

              get_ast(inner_blocks, current_context^);
            },
            items,
          );
        ast := List.append(ast^, [UnorderedList(items_ast)]);
      | _ => ()
      };

      current_position := current_position^ + 1;
    };

    ast^;
  };

let rec get_context: AST.t => list(BlockContext.reference) =
  source => {
    let current_position: ref(int) = ref(0);
    let context: ref(list(BlockContext.reference)) = ref([]);

    while (current_position^ < List.length(source)) {
      switch (List.nth(source, 0)) {
      | ReferenceResolution(data) =>
        context :=
          List.append(
            context^,
            [(data.referenceId, data.linkUrl, data.linkTitle)],
          )
      | Quote(blocks) =>
        context := List.append(context^, get_context(blocks))
      | UnorderedList(items) =>
        List.iter(
          (item: AST.unordered_list_item) =>
            context := List.append(context^, get_context(item)),
          items,
        )
      | OrderedList(items) =>
        List.iter(
          (item: AST.ordered_list_item) =>
            context := List.append(context^, get_context(item.blocks)),
          items,
        )
      | _ => ()
      };

      current_position := current_position^ + 1;
    };

    context^;
  };

let rec get_markdown: (AST.t, BlockContext.t, bool) => list(string) =
  (ast, context, is_inner) => {
    let current_position: ref(int) = ref(0);
    let lines: ref(list(string)) = ref([]);

    while (current_position^ < List.length(ast)) {
      let current_block = List.nth(ast, current_position^);

      switch (current_block) {
      | Paragraph(spans) =>
        lines := List.append(lines^, Paragraph.get_markdown(spans, context))
      | Heading(level, spans) =>
        lines :=
          List.append(
            lines^,
            AtxStyleHeader.get_markdown(level, spans, context),
          )
      | Code(text) =>
        lines :=
          List.append(
            lines^,
            List.map(line => "    " ++ line, Util.initLines(text)),
          )
      | Quote(blocks) =>
        lines :=
          List.append(
            lines^,
            List.map(
              (line: string) => ">" ++ line,
              get_markdown(blocks, context, true),
            ),
          )
      | HorizontalRule => lines := List.append(lines^, ["***"])
      | Null => lines := List.append(lines^, ["\n"])
      | ReferenceResolution(data) =>
        lines := List.append(lines^, ReferenceResolution.get_markdown(data))
      | UnorderedList(items) =>
        let unordered_list_lines: ref(list(string)) = ref([]);

        List.iter(
          (item: AST.unordered_list_item) =>
            unordered_list_lines :=
              List.append(
                unordered_list_lines^,
                List.mapi(
                  (index: int, line: string) =>
                    if (index == 0) {
                      "- " ++ line;
                    } else {
                      "  " ++ line;
                    },
                  get_markdown(item, context, true),
                ),
              ),
          items,
        );

        lines := List.append(lines^, unordered_list_lines^);
      | OrderedList(items) =>
        let ordered_list_lines: ref(list(string)) = ref([]);

        List.iter(
          (item: AST.ordered_list_item) =>
            ordered_list_lines :=
              List.append(
                ordered_list_lines^,
                List.mapi(
                  (index: int, line: string) =>
                    if (index == 0) {
                      string_of_int(item.number) ++ ". " ++ line;
                    } else {
                      "  " ++ line;
                    },
                  get_markdown(item.blocks, context, true),
                ),
              ),
          items,
        );

        lines := List.append(lines^, ordered_list_lines^);
      };

      if (! is_inner) {
        lines := List.append(lines^, ["\n"]);
      };

      current_position := current_position^ + 1;
    };

    lines^;
  };
