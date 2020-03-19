open SpanTypes;

module Regex = {
  let backticks_followed_by_non_backticks = Js.Re.fromString("^(`+)([^`].*)"); /* Capture group one: opening_backticks_string - capture group two: residual_code_span_sequence */
  let backticks_at_the_end = Js.Re.fromString("^(`+)$"); /* Capture group one: opening_backticks_string */

  let residual_code_span_sequence_followed_by_non_backtick =
    Js.Re.fromString("^([^`]+)(`+)([^`].*)");
  let residual_code_span_sequence_at_the_end =
    Js.Re.fromString("^([^`]+)(`+)$");
};

let stack: ref(Stack.t(SpanTypes.t)) = ref(Stack.create());

type code_span = {
  opening_backticks_string: string,
  opening_backticks_count: int,
  residual_code_span_sequence: option(string),
};

let identify: (Stack.t(SpanTypes.t), string, int) => int =
  (new_stack, input_character_sequence, current_position) => {
    stack := new_stack;

    let remaining_character_sequence =
      String.sub(
        input_character_sequence,
        current_position,
        String.length(input_character_sequence) - current_position,
      );
    let code_content_length: ref(int) = ref(0);
    let opening_backticks_string: ref(string) = ref("");
    let opening_backticks_count: ref(int) = ref(0);
    let closing_code_string: ref(option(string)) = ref(None);
    let residual_code_span_sequence: ref(option(string)) = ref(None);
    let code: ref(string) = ref("");
    let matched = ref(false);

    if (! matched^) {
      let match =
        Js.String.match(
          Regex.backticks_followed_by_non_backticks,
          remaining_character_sequence,
        );

      switch (match) {
      | None => ()
      | Some(captures) =>
        opening_backticks_string := captures[1];
        opening_backticks_count := String.length(captures[1]);
        residual_code_span_sequence := Some(captures[2]);
        matched := true;
      };
    };

    if (! matched^) {
      let match =
        Js.String.match(
          Regex.backticks_at_the_end,
          remaining_character_sequence,
        );

      switch (match) {
      | None => ()
      | Some(captures) =>
        opening_backticks_string := captures[1];
        opening_backticks_count := String.length(captures[1]);
        matched := true;
      };
    };

    while (residual_code_span_sequence^ != None && closing_code_string^ == None) {
      switch (residual_code_span_sequence^) {
      | None => ()
      | Some(sequence) =>
        let code_fragment_string = ref("");
        let backticks_fragment_string = ref("");
        let matched = ref(false);

        if (! matched^) {
          let match =
            Js.String.match(
              Regex.residual_code_span_sequence_followed_by_non_backtick,
              sequence,
            );

          switch (match) {
          | None => ()
          | Some(pattern) =>
            code_fragment_string := pattern[1];
            code := code^ ++ code_fragment_string^;
            backticks_fragment_string := pattern[2];
            residual_code_span_sequence := Some(pattern[3]);
            matched := true;
          };
        };

        if (! matched^) {
          let match =
            Js.String.match(
              Regex.residual_code_span_sequence_at_the_end,
              sequence,
            );

          switch (match) {
          | None => ()
          | Some(pattern) =>
            code_fragment_string := pattern[1];
            code := code^ ++ code_fragment_string^;
            backticks_fragment_string := pattern[2];
            residual_code_span_sequence := None;
            matched := true;
          };
        };

        if (! matched^) {
          residual_code_span_sequence := None;
        };

        if (String.length(backticks_fragment_string^)
            == opening_backticks_count^) {
          code_content_length := String.length(code^);
          closing_code_string := Some(backticks_fragment_string^);
        } else {
          code := code^ ++ backticks_fragment_string^;
          code_content_length := String.length(code^);
        };
      };
    };

    switch (closing_code_string^) {
    | Some(_value) =>
      let code_span_length =
        opening_backticks_count^ * 2 + code_content_length^;

      Stack.push(SpanTag(OpeningCodeSpanTag), stack^);
      Stack.push(SpanTag(TextFragment(Util.trim(code^))), stack^); /* TODO: trim string */
      Stack.push(SpanTag(ClosingCodeTag), stack^);

      code_span_length;
    | None =>
      Stack.push(SpanTag(TextFragment(opening_backticks_string^)), stack^);

      opening_backticks_count^;
    };
  };

let rec find_smallest_possible_length: (string, int) => int =
  (source, number) => {
    let find_regex = Js.Re.fromString("`{" ++ string_of_int(number) ++ "}");

    switch (Js.String.match(find_regex, source)) {
    | None => number
    | Some(_) => find_smallest_possible_length(source, number + 1)
    };
  };

let get_markdown: string => string =
  data =>
    if (Js.Re.test_(Js.Re.fromString("[^`]`[^`]|^`|`$"), data)) {
      let smallest_possible_length = find_smallest_possible_length(data, 2);

      let pattern: ref(string) = ref("");

      for (_count in 0 to smallest_possible_length) {
        pattern := pattern^ ++ "`";
      };

      pattern^ ++ data ++ pattern^;
    } else {
      "`" ++ data ++ "`";
    };