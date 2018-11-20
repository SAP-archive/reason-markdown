open SpanTypes;

/* Instances used to process the input */

let stack: ref(Stack.t(SpanTypes.t)) = ref(Stack.create());

/* Regular expressions used to determine which element method shall be called */
module Regex = {
  let escape_symbol = Js.Re.fromString("^\\\\");

  let escapable = Js.Re.fromString("^\\\\([*]|[_]|[`]|\\[|\\]|[!]|[<]|\\\\)");

  let escaped_asterisk = Js.Re.fromString("^\\\\[*]");

  let escaped_underscore = Js.Re.fromString("^\\\\[_]");

  let escaped_backtick = Js.Re.fromString("^\\\\[`]");

  let escaped_square_bracket_open = Js.Re.fromString("^\\\\\\[");

  let escaped_square_backet_close = Js.Re.fromString("^\\\\\\]");

  let escaped_question_mark = Js.Re.fromString("^\\\\[!]");

  let escaped_left_angle_bracket_open = Js.Re.fromString("^\\\\[<]");

  let asterisk = Js.Re.fromString("^[*]");

  let underscore = Js.Re.fromString("^[_]");

  let backtick = Js.Re.fromString("^[`]");

  let square_bracket_open = Js.Re.fromString("^\\[");

  let square_bracket_close = Js.Re.fromString("^\\]");

  let maybe_image_link = Js.Re.fromString("^[!]\\[");

  let angle_bracket_open = Js.Re.fromString("^<");

  let line_break = Js.Re.fromString("^\n");

  let markdown_line_break = Js.Re.fromString(" {2,}$");
};

/* Method to transform all elements of type Node in a list of type t to type SpanTag */

let stack_to_list: Stack.t('a) => list('a) =
  stack => {
    let new_list: ref(list('a)) = ref([]);

    Stack.iter(element => new_list := [element, ...new_list^], stack);

    new_list^;
  };

let rec transform_nodes_to_spans: list(SpanTypes.t) => list(SpanTypes.span) =
  source =>
    switch (source) {
    | [] => []
    | [head, ...tail] =>
      switch (head) {
      | Node(data) => [
          TextFragment(data.tagString),
          ...transform_nodes_to_spans(tail),
        ]
      | SpanTag(span) => [span, ...transform_nodes_to_spans(tail)]
      }
    };


let merge_lines: list(string) => string = source => {
  let result: ref(string) = ref("");
  let current_position: ref(int) = ref(0);

  while (current_position^ < List.length(source)) {
    let current_string = List.nth(source, current_position^);
    let matches_line_break = Js.String.match(Regex.markdown_line_break, current_string);

    switch matches_line_break {
    | None => 
      result := String.concat("", [result^, current_string, (current_position^ == List.length(source) - 1 ? "" : " ")]);
    | Some(_) => 
      let final = Js.String.replaceByRe(Regex.markdown_line_break, current_position^ == List.length(source) - 1? "" : "\n", current_string);
      result := String.concat("", [result^, final]);
    };

    current_position := current_position^ + 1;
  };

  result^;
};

/* Main loop which calls element methods to recognize span tags */
let identify: list(string) => list(SpanTypes.span) =
  lines => {
    let input_character_sequence = merge_lines(lines);
    Stack.clear(stack^);
    let input_length = String.length(input_character_sequence);
    let remaining_characters: ref(string) = ref(input_character_sequence);
    let current_position: ref(int) = ref(0);

    while (String.length(input_character_sequence) > current_position^) {
      remaining_characters :=
        String.sub(
          input_character_sequence,
          current_position^,
          input_length - current_position^,
        );
      /* The main loop decides which element methody are called by looking at the first characters */

      let remaining_length = String.length(remaining_characters^);

      if (Js.Re.test(remaining_characters^, Regex.escape_symbol)) {
        let escape_match =
          Js.String.match(Regex.escapable, remaining_characters^);

        switch (escape_match) {
        | None =>
          Stack.push(SpanTag(TextFragment("\\")), stack^);
          current_position := current_position^ + 1;
        | Some(captures) =>
          let escaped_symbol = captures[1];
          let capture_length = String.length(captures[0]);

          Stack.push(SpanTag(TextFragment(escaped_symbol)), stack^);
          current_position := current_position^ + capture_length;
        };
      } else if (Js.Re.test(remaining_characters^, Regex.square_bracket_open)
                 || Js.Re.test(
                      remaining_characters^,
                      Regex.square_bracket_close,
                    )) {
        let consumed_characters =
          Link.identify(stack^, input_character_sequence, current_position^);
        current_position := current_position^ + consumed_characters;
      } else if (Js.Re.test(remaining_characters^, Regex.asterisk)
                 || Js.Re.test(remaining_characters^, Regex.underscore)) {
        let consumed_characters =
          Emphasis.identify(
            stack^,
            input_character_sequence,
            current_position^,
          );
        current_position := current_position^ + consumed_characters;
      } else if (Js.Re.test(remaining_characters^, Regex.backtick)) {
        let consumed_characters =
          CodeSpan.identify(
            stack^,
            input_character_sequence,
            current_position^,
          );
        current_position := current_position^ + consumed_characters;
      } else if (Js.Re.test(remaining_characters^, Regex.maybe_image_link)) {
        let consumed_characters =
          Image.identify(stack^, input_character_sequence, current_position^);
        current_position := current_position^ + consumed_characters;
      } else if (Js.Re.test(remaining_characters^, Regex.line_break)) {
        Stack.push(SpanTag(LineBreak), stack^);
        current_position := current_position^ + 1;
      } else {
        Stack.push(
          SpanTag(TextFragment(String.sub(remaining_characters^, 0, 1))),
          stack^,
        );
        current_position := current_position^ + 1;
      };
    };

    let spans = transform_nodes_to_spans(stack_to_list(stack^));
    Stack.clear(stack^);

    spans;
  };

let read_emphasis:
  (list(SpanTypes.span), int) =>
  (list(SpanTypes.span), string, emphasis_type, int) =
  (source, position) => {
    let current_position: ref(int) = ref(position + 1);
    let open_emphasis_tags: ref(int) = ref(1);
    let content: ref(list(SpanTypes.span)) = ref([]);
    let emphasis_type: ref(SpanTypes.emphasis_type) = ref(EmphaticStress);
    let emphasis_pattern: ref(string) = ref("*");

    while (current_position^ < List.length(source) && open_emphasis_tags^ > 0) {
      let current_span = List.nth(source, current_position^);

      switch (current_span) {
      | ClosingEmphasisTag(pattern, type_) =>
        open_emphasis_tags := open_emphasis_tags^ - 1;
        if (open_emphasis_tags^ == 0) {
          emphasis_type := type_;
          emphasis_pattern := pattern;
        } else {
          content :=
            List.append(content^, [ClosingEmphasisTag(pattern, type_)]);
        };

        current_position := current_position^ + 1;
      | OpeningEmphasisSpanTag =>
        open_emphasis_tags := open_emphasis_tags^ + 1;
        content := List.append(content^, [OpeningEmphasisSpanTag]);
        current_position := current_position^ + 1;
      | span =>
        content := List.append(content^, [span]);
        current_position := current_position^ + 1;
      };
    };

    (
      content^,
      emphasis_pattern^,
      emphasis_type^,
      current_position^ - position,
    );
  };

let read_text: (list(SpanTypes.span), int) => (AST.span, int) =
  (source, position) => {
    let current_position: ref(int) = ref(position);
    let escape: ref(bool) = ref(false);
    let current_text: ref(string) = ref("");

    while (current_position^ < List.length(source) && ! escape^) {
      let current_span = List.nth(source, current_position^);

      switch (current_span) {
      | TextFragment(text) =>
        current_text := current_text^ ++ text;
        current_position := current_position^ + 1;
      | _ => escape := true
      };
    };

    (AST.Text(current_text^), current_position^ - position);
  };

let read_code: (list(SpanTypes.span), int) => (AST.span, int) =
  (source, position) => {
    let current_position: ref(int) = ref(position + 1);
    let escape: ref(bool) = ref(false);
    let code: ref(string) = ref("");

    while (current_position^ < List.length(source) && ! escape^) {
      let current_span = List.nth(source, current_position^);

      switch (current_span) {
      | TextFragment(text) =>
        code := code^ ++ text;
        current_position := current_position^ + 1;
      | ClosingCodeTag =>
        current_position := current_position^ + 1;
        escape := true;
      };
    };

    (AST.Code(code^), current_position^ - position);
  };

let get_reference:
  (string, BlockContext.t) => (option(string), option(string)) =
  (this_reference_id, context) =>
    switch (
      List.find(
        ((reference_id, _url, _title)) => reference_id == this_reference_id,
        context.references,
      )
    ) {
    | (_, url, title) => (Some(url), Some(title))
    | exception Not_found => (None, None)
    };

let read_image:
  (list(SpanTypes.span), int, BlockContext.t) => (AST.span, int) =
  (source, position, context) =>
    switch (List.nth(source, position)) {
    | ImageSpanTag(data) =>
      let image_data: ref(AST.image_data) =
        ref({
          AST.alternativeText: data.altText,
          AST.title: data.title,
          AST.url: None,
        });

      switch (data.url) {
      | None =>
        switch (data.referenceId) {
        | None => ()
        | Some(reference_id) =>
          let (link_url, link_title) = get_reference(reference_id, context);
          image_data := {...image_data^, url: link_url, title: link_title};
        }
      | Some(url) =>
        /* Transform url correctly */
        image_data := {...image_data^, url: Some(url)}
      };

      (AST.Image(image_data^), 1);
    | _ => (AST.Text(""), 1)
    };

let read_link:
  (list(SpanTypes.span), int, BlockContext.t) =>
  (list(SpanTypes.span), option(string), int) =
  (source, position, context) => {
    let current_position: ref(int) = ref(position + 1);
    let escape: ref(bool) = ref(false);
    let content: ref(list(SpanTypes.span)) = ref([]);
    let url: ref(option(string)) = ref(None);

    while (current_position^ < List.length(source) && ! escape^) {
      let current_span = List.nth(source, current_position^);

      switch (current_span) {
      | ClosingLinkTag(data) =>
        current_position := current_position^ + 1;
        escape := true;

        switch (data.url) {
        | None =>
          switch (data.referenceId) {
          | None => url := None
          | Some(reference_id) =>
            let (link_url, _link_title) =
              get_reference(reference_id, context);
            url := link_url;
          }
        | Some(this_url) => url := Some(this_url)
        };
      | span =>
        current_position := current_position^ + 1;
        content := List.append(content^, [span]);
      };
    };

    (content^, url^, current_position^ - position);
  };

/* Takes an array of SpanTags and transforms it to a AST */
let rec get_ast: (list(SpanTypes.span), BlockContext.t) => AST.spans =
  (source, context) => {
    let current_position: ref(int) = ref(0);
    let ast: ref(AST.spans) = ref([]);

    while (current_position^ < List.length(source)) {
      let current_span = List.nth(source, current_position^);

      switch (current_span) {
      | LineBreak => 
        ast := List.append(ast^, [AST.LineBreak]);
        current_position := current_position^ + 1;
      | TextFragment(text) =>
        let (text, consumed_span_count) =
          read_text(source, current_position^);

        ast := List.append(ast^, [text]);
        current_position := current_position^ + consumed_span_count;
      | OpeningEmphasisSpanTag =>
        let (content, pattern, emphasis_type, consumed_span_count) =
          read_emphasis(source, current_position^);

        let real_content = get_ast(content, context);

        switch (emphasis_type) {
        | EmphaticStress =>
          ast :=
            List.append(ast^, [AST.EmphaticStress(pattern, real_content)])
        | StrongImportance =>
          ast :=
            List.append(ast^, [AST.StrongImportance(pattern, real_content)])
        | EmphaticStressStrongImportance =>
          ast :=
            List.append(
              ast^,
              [AST.EmphaticStressStrongImportance(pattern, real_content)],
            )
        };

        current_position := current_position^ + consumed_span_count;
      | OpeningCodeSpanTag =>
        let (code, consumed_span_count) =
          read_code(source, current_position^);
        ast := List.append(ast^, [code]);
        current_position := current_position^ + consumed_span_count;
      | ImageSpanTag(_) =>
        let (image, consumed_span_count) =
          read_image(source, current_position^, context);
        ast := List.append(ast^, [image]);
        current_position := current_position^ + consumed_span_count;
      | OpeningLinkSpanTag =>
        let (content, url, consumed_span_count) =
          read_link(source, current_position^, context);
        ast :=
          List.append(
            ast^,
            [AST.Link({title: get_ast(content, context), url})],
          );
        current_position := current_position^ + consumed_span_count;
      | _ => raise(Not_found)
      };
    };

    ast^;
  };

let rec get_markdown: (AST.spans, BlockContext.t) => string =
  (spans, context) => {
    let current_position: ref(int) = ref(0);
    let output: ref(string) = ref("");

    while (current_position^ < List.length(spans)) {
      let current_span = List.nth(spans, current_position^);

      switch (current_span) {
      | LineBreak => output := output^ ++ "  \n"
      | Text(content) => output := output^ ++ content
      | EmphaticStress(pattern, content)
      | StrongImportance(pattern, content)
      | EmphaticStressStrongImportance(pattern, content) =>
        output :=
          output^ ++ pattern ++ get_markdown(content, context) ++ pattern
      | Code(content) => output := output^ ++ CodeSpan.get_markdown(content)
      | Image(data) => output := output^ ++ Image.get_markdown(data, context)
      | Link(data) =>
        output :=
          output^
          ++ Link.get_markdown(
               get_markdown(data.title, context),
               data.url,
               context,
             )
      };

      current_position := current_position^ + 1;
    };

    output^;
  };
