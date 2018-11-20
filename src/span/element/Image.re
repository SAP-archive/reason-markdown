open SpanTypes;

module Regex = {
  let image_tag_starter_pattern =
    Js.Re.fromString("^!\\[(([^\\\\\\[\\]`]|\\\\.)*)(\\].*)");

  let residual_image_sequence_reference_id =
    Js.Re.fromString("^\\]\\s*\\[(([^\\\\\\[\\]`]|\\\\.)*)\\]");

  let residual_image_sequence_url_indicator = Js.Re.fromString("^\\]\\s*\\(");

  let residual_image_sequence_url_without_angle_brackets =
    Js.Re.fromString("^\\]\\s*\\(\\s*([^()<>`\\s]+)(.*[)\\s])");

  let residual_image_sequence_url_within_angle_brackets =
    Js.Re.fromString("^\\]\\s*\\(\\s*[<]([^<>`]*)[>](.*[)])");

  let residual_attributes_empty = Js.Re.fromString("^\\s*\\)");

  let residual_attributes_not_empty =
    Js.Re.fromString(
      "^\\s*(\"(([^\"\\\\`]|\\\\.)*)\"|'(([^'\\\\`]|\\\\.)*)')\\s*\\)",
    );

  let residual_image_sequence_empty_ref =
    Js.Re.fromString("^(\\]\\s*\\[\\s*\\])");
};

let stack: ref(Stack.t(SpanTypes.t)) = ref(Stack.create());

let identify: (Stack.t(SpanTypes.t), string, int) => int =
  (new_stack, input_character_sequence, current_position) => {
    stack := new_stack;

    let remaining_characters =
      String.sub(
        input_character_sequence,
        current_position,
        String.length(input_character_sequence) - current_position,
      );

    let matches_image_starter_pattern =
      Js.String.match(Regex.image_tag_starter_pattern, remaining_characters);

    switch (matches_image_starter_pattern) {
    | None =>
      Stack.push(SpanTag(TextFragment("![")), stack^);
      2;
    | Some(captures) =>
      let image_alt_text_string = captures[1];
      let residual_image_sequence = captures[3];
      let alt_text_pattern_match_length =
        Js.String.indexOf(residual_image_sequence, remaining_characters);

      let match_reference_id =
        Js.String.match(
          Regex.residual_image_sequence_reference_id,
          residual_image_sequence,
        );

      switch (match_reference_id) {
      | Some(captures) =>
        let reference_id = Util.simplify(captures[1]);
        let image_ref_close_sequence_length = String.length(captures[0]);
        let image_ref_tag_length =
          alt_text_pattern_match_length + image_ref_close_sequence_length;

        Stack.push(
          SpanTag(
            ImageSpanTag({
              title: None,
              altText: image_alt_text_string,
              referenceId: Some(reference_id),
              url: None,
            }),
          ),
          stack^,
        );

        image_ref_tag_length;
      | None =>
        if (Js.Re.test(
              residual_image_sequence,
              Regex.residual_image_sequence_url_indicator,
            )) {
          let unprocessed_image_source_string = ref("");
          let image_source_pattern_match_length = ref(0);
          let residual_image_attribute_sequence = ref("");
          let attributes_string: ref(option(string)) = ref(None);
          let image_attributes_pattern_match_length = ref(0);
          let matched = ref(false);

          if (! matched^) {
            let matches_without_brackets =
              Js.String.match(
                Regex.residual_image_sequence_url_without_angle_brackets,
                residual_image_sequence,
              );

            switch (matches_without_brackets) {
            | None => ()
            | Some(captures) =>
              unprocessed_image_source_string := captures[1];
              residual_image_attribute_sequence := captures[2];
              image_source_pattern_match_length :=
                Js.String.indexOf(
                  residual_image_attribute_sequence^,
                  residual_image_sequence,
                );
              matched := true;
            };
          };

          if (! matched^) {
            let matches_within_brackets =
              Js.String.match(
                Regex.residual_image_sequence_url_within_angle_brackets,
                residual_image_sequence,
              );

            switch (matches_within_brackets) {
            | None => ()
            | Some(captures) =>
              unprocessed_image_source_string := captures[1];
              residual_image_attribute_sequence := captures[2];
              image_source_pattern_match_length :=
                Js.String.indexOf(
                  residual_image_attribute_sequence^,
                  residual_image_sequence,
                );
              matched := true;
            };
          };

          if (matched^) {
            let matched = ref(false);

            if (! matched^) {
              let matches_empty_title =
                Js.String.match(
                  Regex.residual_attributes_empty,
                  residual_image_attribute_sequence^,
                );

              switch (matches_empty_title) {
              | None => ()
              | Some(captures) =>
                image_attributes_pattern_match_length :=
                  String.length(captures[0]);
                attributes_string := None;
                matched := true;
              };
            };

            if (! matched^) {
              let matches_title =
                Js.String.match(
                  Regex.residual_attributes_not_empty,
                  residual_image_attribute_sequence^,
                );

              switch (matches_title) {
              | None => ()
              | Some(captures) =>
                image_attributes_pattern_match_length :=
                  String.length(captures[0]);
                attributes_string :=
                  Some(
                    Js.String.replaceByRe(
                      Js.Re.fromStringWithFlags("\"", "g"),
                      "",
                      captures[1],
                    ),
                  );
                matched := true;
              };
            };
          };

          let image_src_tag_length =
            alt_text_pattern_match_length
            + image_source_pattern_match_length^
            + image_attributes_pattern_match_length^;

          Stack.push(
            SpanTag(
              ImageSpanTag({
                title: attributes_string^,
                altText: image_alt_text_string,
                url: Some(unprocessed_image_source_string^),
                referenceId: None,
              }),
            ),
            stack^,
          );

          image_src_tag_length;
        } else {
          let matches_empty_ref =
            Js.String.match(
              Regex.residual_image_sequence_empty_ref,
              residual_image_sequence,
            );

          switch (matches_empty_ref) {
          | None =>
            let image_ref_tag_length = alt_text_pattern_match_length + 1;
            let reference_id = Util.simplify(image_alt_text_string);

            Stack.push(
              SpanTag(
                ImageSpanTag({
                  title: None,
                  altText: image_alt_text_string,
                  url: None,
                  referenceId: Some(reference_id),
                }),
              ),
              stack^,
            );

            image_ref_tag_length;
          | Some(captures) =>
            let image_ref_tag_length =
              alt_text_pattern_match_length + String.length(captures[0]);
            let reference_id = Util.simplify(image_alt_text_string);

            Stack.push(
              SpanTag(
                ImageSpanTag({
                  title: None,
                  altText: image_alt_text_string,
                  url: None,
                  referenceId: Some(reference_id),
                }),
              ),
              stack^,
            );

            image_ref_tag_length;
          };
        }
      };
    };
  };

let rec find_reference_id:
  (list(BlockContext.reference), string) => option(string) =
  (reference, url) =>
    switch (reference) {
    | [(reference_id, link_url, link_title), ...tail] =>
      if (link_url == url) {
        Some(reference_id);
      } else {
        find_reference_id(tail, url);
      }
    | [] => None
    };

let get_markdown: (AST.image_data, BlockContext.t) => string =
  (data, context) =>
    switch (data.url) {
    | None =>
      switch (data.title) {
      | None => "![" ++ data.alternativeText ++ "]()"
      | Some(title) =>
        "![" ++ data.alternativeText ++ "](<> \"" ++ title ++ "\")"
      }
    | Some(url) =>
      let reference = find_reference_id(context.references, url);

      switch (reference) {
      | None =>
        switch (data.title) {
        | None => "![" ++ data.alternativeText ++ "](" ++ url ++ ")"
        | Some(title) =>
          "!["
          ++ data.alternativeText
          ++ "]("
          ++ url
          ++ " \""
          ++ title
          ++ "\")"
        }
      | Some(reference_id) =>
        "![" ++ data.alternativeText ++ "][" ++ reference_id ++ "]"
      };
    };
