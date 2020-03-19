open SpanTypes;

module Regex = {
  let square_bracket_open = Js.Re.fromString("^\\[");

  let square_bracket_close = Js.Re.fromString("^\\]");

  let reference_id_link =
    Js.Re.fromString("^\\]\\s*\\[(([^\\\\\\[\\]\\`]|\\\\.)+)\\]");

  let unprocessed_url_without_angle_brackets =
    Js.Re.fromString("^\\]\\s*\\(\\s*([^()<>`\\s]+)([\\)\\s].*)");

  let unprocessed_url_with_angle_brackets =
    Js.Re.fromString("^\\]\\s*\\(\\s*<([^<>`]*)>([)\\s].*)");

  let residual_attribute_just_closing = Js.Re.fromString("^\\s*\\)");

  let residual_attribute_title =
    Js.Re.fromString(
      "^\\s*(\"(([^\\\\\"`]|\\\\.)*)\"|'(([^\\\\'`]|\\\\.)*)')\\s*\\)",
    );

  let empty_ref_pattern = Js.Re.fromString("^(\\]\\s*\\[\\s*\\])");
};

let stack: ref(Stack.t(SpanTypes.t)) = ref(Stack.create());

let top_link_node_exists: unit => bool =
  () => {
    let exists: ref(bool) = ref(false);

    Stack.iter(
      element =>
        switch (element) {
        | SpanTypes.Node(data) =>
          switch (data.nodeType) {
          | SpanTypes.Link => exists := true
          | _ => ()
          }
        | _ => ()
        },
      stack^,
    );

    exists^;
  };

let pop_enclosed_elements: unit => list(SpanTypes.t) =
  () => {
    let enclosed_elements: ref(list(SpanTypes.t)) = ref([]);
    let ready = ref(false);

    while (! Stack.is_empty(stack^) && ! ready^) {
      let element = Stack.pop(stack^);

      switch (element) {
      | Node(data) =>
        switch (data.nodeType) {
        | Link =>
          Stack.push(SpanTag(OpeningLinkSpanTag), stack^);
          ready := true;
        | _ =>
          enclosed_elements :=
            [SpanTag(TextFragment(data.tagString)), ...enclosed_elements^]
        }
      | c => enclosed_elements := [c, ...enclosed_elements^]
      };
    };

    enclosed_elements^;
  };

let identify_reference_id_link: string => option(int) =
  input_character_sequence => {
    let reference_id_match =
      Js.String.match(Regex.reference_id_link, input_character_sequence);

    switch (reference_id_match) {
    | None => None
    | Some(reference_id_captures) =>
      let closing_tag_length = String.length(reference_id_captures[0]);

      let enclosed_elements = pop_enclosed_elements();

      List.iter(element => Stack.push(element, stack^), enclosed_elements);

      let link: link_data = {
        title: None,
        url: None,
        referenceId: Some(reference_id_captures[1]) /* Simplify! */
      };

      Stack.push(SpanTag(ClosingLinkTag(link)), stack^);

      Some(closing_tag_length);
    };
  };

let identify_residual_link_attribute: string => option((string, int)) =
  residual_link_attribute_sequence => {
    let matches_just_closing =
      Js.String.match(
        Regex.residual_attribute_just_closing,
        residual_link_attribute_sequence,
      );

    switch (matches_just_closing) {
    | Some(captures) => Some(("", String.length(captures[0])))
    | None =>
      let matches_with_title =
        Js.String.match(
          Regex.residual_attribute_title,
          residual_link_attribute_sequence,
        );

      switch (matches_with_title) {
      | None => None
      | Some(captures) => Some((captures[1], String.length(captures[0])))
      };
    };
  };

let identify_unprocessed_url: string => option(int) =
  input_character_sequence => {
    let matches_without_brackets =
      Js.String.match(
        Regex.unprocessed_url_without_angle_brackets,
        input_character_sequence,
      );

    switch (matches_without_brackets) {
    | None =>
      let matches_with_brackets =
        Js.String.match(
          Regex.unprocessed_url_with_angle_brackets,
          input_character_sequence,
        );

      switch (matches_with_brackets) {
      | None => None
      | Some(with_brackets_captures) =>
        let unprocessed_url_string = with_brackets_captures[1];

        switch (identify_residual_link_attribute(with_brackets_captures[2])) {
        | None => None
        | Some((unprocessed_title, attributes_pattern_match_length)) =>
          let closing_tag_length =
            String.length(with_brackets_captures[0])
            - String.length(with_brackets_captures[2])
            + attributes_pattern_match_length;

          let enclosed_elements = pop_enclosed_elements();

          List.iter(
            element => Stack.push(element, stack^),
            enclosed_elements,
          );

          let link: link_data = {
            title: Some(unprocessed_title),
            url: Some(unprocessed_url_string),
            referenceId: None,
          };

          Stack.push(SpanTag(ClosingLinkTag(link)), stack^);

          Some(closing_tag_length);
        };
      };
    | Some(without_brackets_captures) =>
      let unprocessed_url_string = without_brackets_captures[1];

      switch (identify_residual_link_attribute(without_brackets_captures[2])) {
      | None => None
      | Some((unprocessed_title, attributes_pattern_match_length)) =>
        let closing_tag_length =
          String.length(without_brackets_captures[0])
          - String.length(without_brackets_captures[2])
          + attributes_pattern_match_length;

        let enclosed_elements = pop_enclosed_elements();

        List.iter(element => Stack.push(element, stack^), enclosed_elements);

        let link: link_data = {
          title: Some(unprocessed_title),
          url: Some(unprocessed_url_string),
          referenceId: None,
        };

        Stack.push(SpanTag(ClosingLinkTag(link)), stack^);

        Some(closing_tag_length);
      };
    };
  };

let identify_empty_ref: string => int =
  input_character_sequence => {
    let matches_empty_ref =
      Js.String.match(Regex.empty_ref_pattern, input_character_sequence);
    let closing_tag_length = ref(0);

    switch (matches_empty_ref) {
    | None => closing_tag_length := 1
    | Some(captures) => closing_tag_length := String.length(captures[0])
    };

    let enclosed_elements = pop_enclosed_elements();

    List.iter(element => Stack.push(element, stack^), enclosed_elements);

    let link: link_data = {title: None, url: None, referenceId: None};

    Stack.push(SpanTag(ClosingLinkTag(link)), stack^);

    closing_tag_length^;
  };

let identify: (Stack.t(SpanTypes.t), string, int) => int =
  (new_stack, input_character_sequence, current_position) => {
    let remaining_sequence =
      String.sub(
        input_character_sequence,
        current_position,
        String.length(input_character_sequence) - current_position,
      );

    stack := new_stack;
    if (Js.Re.test_(Regex.square_bracket_open, remaining_sequence)) {
      Stack.push(
        Node({
          tagString: "[",
          nodeType: Link,
          linkedContentStart: Some(-1),
          htmlTagName: None,
        }),
        stack^,
      );
      1;
    } else if (Js.Re.test_(Regex.square_bracket_close, remaining_sequence)) {
      if (top_link_node_exists()) {
        switch (identify_reference_id_link(remaining_sequence)) {
        | Some(result) => result
        | None =>
          switch (identify_unprocessed_url(remaining_sequence)) {
          | Some(result) => result
          | None => identify_empty_ref(remaining_sequence)
          }
        };
      } else {
        Stack.push(SpanTag(TextFragment("]")), stack^);

        1;
      };
    } else {
      raise(Not_found);
    };
  };

let rec find_reference_id:
  (list(BlockContext.reference), string) => option(string) =
  (reference, url) =>
    switch (reference) {
    | [(reference_id, link_url, _link_title), ...tail] =>
      if (link_url == url) {
        Some(reference_id);
      } else {
        find_reference_id(tail, url);
      }
    | [] => None
    };

let get_markdown: (string, option(string), BlockContext.t) => string =
  (title, url, context) =>
    switch (url) {
    | None => "[" ++ title ++ "]()"
    | Some(url) =>
      let reference_id = find_reference_id(context.references, url);

      switch (reference_id) {
      | None => "[" ++ title ++ "](<" ++ url ++ ">)"
      | Some(id) => "[" ++ title ++ "][" ++ id ++ "]"
      };
    };
