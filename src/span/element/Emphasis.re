open SpanTypes;

module Regex = {
  let emphasis_indicator = Js.Re.fromString("^([*_]+)");

  let match_at_end = Js.Re.fromString("^([*_]+)$"); /* Captures the emphasis indicator string. right_fringe_rank = 0 */

  let match_in_middle = Js.Re.fromString("^([*_]+)([^*_])"); /* Captures the emphasis indicator string. right_fringe_rank = emphasis_fringe_rank of second capture group */

  /*
     This Regex matches Unicode characters of the categories Zs, Zl, Zp, Cc, Cf.
     Unicode property support was added in ES 2018 (not usable here, since we are using ES 2015)
   */
  let fringe_rank_zero =
    Js.Re.fromString(
      "^[\\u0000- \\u007f-\\u00a0\\u00ad\\u0600-\\u0603\\u06dd\\u070f\\u1680\\u17b4-\\u17b5\\u180e\\u2000-\\u200f\\u2028-\\u202f\\u205f-\\u2064\\u206a-\\u206f\\u3000\\ufeff\\ufff9-\\ufffb]|\\ud834[\\udd73-\\udd7a]|\\udb40[\\udc01\\udc20-\\udc7f]",
    );

  /*
     This Regex matches Unicode characters of the categories Pc, Pd, Ps, Pe, Pi, Pf, Po, Sc, Sk, Sm, So.
     The Regex looks very ugly, maybe this will be refactored later.
     Unicode property support was added in ES 2018 (not usable here, since we are using ES 2015)
   */
  let fringe_rank_one =
    Js.Re.fromString(
      "^[!-/:-@[-`{-~\\u00A1-\\u00A9\\u00AB-\\u00AC\\u00AE-\\u00B1\\u00B4\\u00B6-\\u00B8\\u00BB\\u00BF\\u00D7\\u00F7\\u02C2-\\u02C5\\u02D2-\\u02DF\\u02E5-\\u02EB\\u02ED\\u02EF-\\u02FF\\u0375\\u037E\\u0384-\\u0385\\u0387\\u03F6\\u0482\\u055A-\\u055F\\u0589-\\u058A\\u05BE\\u05C0\\u05C3\\u05C6\\u05F3-\\u05F4\\u0606-\\u060F\\u061B\\u061E-\\u061F\\u066A-\\u066D\\u06D4\\u06E9\\u06FD-\\u06FE\\u0700-\\u070D\\u07F6-\\u07F9\\u0964-\\u0965\\u0970\\u09F2-\\u09F3\\u09FA\\u0AF1\\u0B70\\u0BF3-\\u0BFA\\u0C7F\\u0CF1-\\u0CF2\\u0D79\\u0DF4\\u0E3F\\u0E4F\\u0E5A-\\u0E5B\\u0F01-\\u0F17\\u0F1A-\\u0F1F\\u0F34\\u0F36\\u0F38\\u0F3A-\\u0F3D\\u0F85\\u0FBE-\\u0FC5\\u0FC7-\\u0FCC\\u0FCE-\\u0FD4\\u104A-\\u104F\\u109E-\\u109F\\u10FB\\u1360-\\u1368\\u1390-\\u1399\\u166D-\\u166E\\u169B-\\u169C\\u16EB-\\u16ED\\u1735-\\u1736\\u17D4-\\u17D6\\u17D8-\\u17DB\\u1800-\\u180A\\u1940\\u1944-\\u1945\\u19DE-\\u19FF\\u1A1E-\\u1A1F\\u1B5A-\\u1B6A\\u1B74-\\u1B7C\\u1C3B-\\u1C3F\\u1C7E-\\u1C7F\\u1FBD\\u1FBF-\\u1FC1\\u1FCD-\\u1FCF\\u1FDD-\\u1FDF\\u1FED-\\u1FEF\\u1FFD-\\u1FFE\\u2010-\\u2027\\u2030-\\u205E\\u207A-\\u207E\\u208A-\\u208E\\u20A0-\\u20B5\\u2100-\\u2101\\u2103-\\u2106\\u2108-\\u2109\\u2114\\u2116-\\u2118\\u211E-\\u2123\\u2125\\u2127\\u2129\\u212E\\u213A-\\u213B\\u2140-\\u2144\\u214A-\\u214D\\u214F\\u2190-\\u23E7\\u2400-\\u2426\\u2440-\\u244A\\u249C-\\u24E9\\u2500-\\u269D\\u26A0-\\u26BC\\u26C0-\\u26C3\\u2701-\\u2704\\u2706-\\u2709\\u270C-\\u2727\\u2729-\\u274B\\u274D\\u274F-\\u2752\\u2756\\u2758-\\u275E\\u2761-\\u2775\\u2794\\u2798-\\u27AF\\u27B1-\\u27BE\\u27C0-\\u27CA\\u27CC\\u27D0-\\u2B4C\\u2B50-\\u2B54\\u2CE5-\\u2CEA\\u2CF9-\\u2CFC\\u2CFE-\\u2CFF\\u2E00-\\u2E2E\\u2E30\\u2E80-\\u2E99\\u2E9B-\\u2EF3\\u2F00-\\u2FD5\\u2FF0-\\u2FFB\\u3001-\\u3004\\u3008-\\u3020\\u3030\\u3036-\\u3037\\u303D-\\u303F\\u309B-\\u309C\\u30A0\\u30FB\\u3190-\\u3191\\u3196-\\u319F\\u31C0-\\u31E3\\u3200-\\u321E\\u322A-\\u3243\\u3250\\u3260-\\u327F\\u328A-\\u32B0\\u32C0-\\u32FE\\u3300-\\u33FF\\u4DC0-\\u4DFF\\uA490-\\uA4C6\\uA60D-\\uA60F\\uA673\\uA67E\\uA700-\\uA716\\uA720-\\uA721\\uA789-\\uA78A\\uA828-\\uA82B\\uA874-\\uA877\\uA8CE-\\uA8CF\\uA92E-\\uA92F\\uA95F\\uAA5C-\\uAA5F\\uFB29\\uFD3E-\\uFD3F\\uFDFC-\\uFDFD\\uFE10-\\uFE19\\uFE30-\\uFE52\\uFE54-\\uFE66\\uFE68-\\uFE6B\\uFF01-\\uFF0F\\uFF1A-\\uFF20\\uFF3B-\\uFF40\\uFF5B-\\uFF65\\uFFE0-\\uFFE6\\uFFE8-\\uFFEE\\uFFFC-\\uFFFD]|\\ud800[\\udd00-\\udd02\\udd37-\\udd3f\\udd79-\\udd89\\udd90-\\udd9b\\uddd0-\\uddfc\\udf9f\\udfd0]|\\ud802[\\udd1f\\udd3f\\ude50-\\ude58]|\\ud809[\\udc70-\\udc73]|\\ud834[\\udc00-\\udcf5\\udd00-\\udd26\\udd29-\\udd64\\udd6a-\\udd6c\\udd83-\\udd84\\udd8c-\\udda9\\uddae-\\udddd\\ude00-\\ude41\\ude45\\udf00-\\udf56]|\\ud835[\\udec1\\udedb\\udefb\\udf15\\udf35\\udf4f\\udf6f\\udf89\\udfa9\\udfc3]|\\ud83c[\\udc00-\\udc2b\\udc30-\\udc93]",
    );
};

/*
   If the current_position is equal to 0, the left_fringe_rank is 0.
   If the current_position is greater than 1, then the left_fringe_rank is the emphasis_fringe_rank
   of the character at the previous position.
 */
/*
   For a given emphasis_indicator_string:
   If left_fringe_rank < right_fringe_rank, then the emphasis_indicator string is said to be LeftFlanking.
   If right_fringe_rank < left_fringe_rank, then the emphasis_indicator string is said to be RightFlanking.
   If both are equal, then the emphasis_indicator_string is said to be NonFlanking.
 */

type flanking =
  | LeftFlanking
  | RightFlanking
  | NonFlanking;

type emphasis_tag_string = {
  text: string,
  constituentCharacter: string,
};

type emphasis_indicator_string = {
  text: string,
  emphasisTagStrings: list(emphasis_tag_string),
  flanking,
  rightFringeRank: int,
  leftFringeRank: int,
};

let compute_flanking: (int, int) => flanking =
  (left_fringe_rank, right_fringe_rank) =>
    if (left_fringe_rank < right_fringe_rank) {
      LeftFlanking;
    } else if (right_fringe_rank < left_fringe_rank) {
      RightFlanking;
    } else {
      NonFlanking;
    };

let get_emphasis_fringe_rank: string => int =
  source => {
    let match_fringe_rank_zero =
      Js.String.match(Regex.fringe_rank_zero, source);

    switch (match_fringe_rank_zero) {
    | Some(_) => 0
    | None =>
      let match_fringe_rank_one =
        Js.String.match(Regex.fringe_rank_one, source);

      switch (match_fringe_rank_one) {
      | Some(_) => 1
      | None => 2
      };
    };
  };

let get_right_fringe_rank: (string, int) => int =
  (source, current_position) => {
    let remaining_input =
      String.sub(
        source,
        current_position,
        String.length(source) - current_position,
      );

    if (Js.Re.test_(Regex.match_at_end, remaining_input)) {
      0;
    } else {
      let match_middle =
        Js.String.match(Regex.match_in_middle, remaining_input);

      switch (match_middle) {
      /*
         Given that the character at the current-position is either * or _ ,
         then the remaining-character-sequence will definitely match one of the patterns.
       */
      | None => raise(Not_found)
      | Some(captures) => get_emphasis_fringe_rank(captures[2])
      };
    };
  };

let get_left_fringe_rank: (string, int) => int =
  (source, current_position) =>
    if (current_position == 0) {
      0;
    } else {
      get_emphasis_fringe_rank(String.sub(source, current_position - 1, 1));
    };

let get_emphasis_tag_strings: string => list(emphasis_tag_string) =
  emphasis_indicator_string => {
    let emphasis_tag_strings: ref(list(emphasis_tag_string)) = ref([]);
    let current_tag: ref(char) = ref(emphasis_indicator_string.[0]);
    let current_tag_string: ref(string) = ref("");
    let current_position: ref(int) = ref(0);

    while (current_position^ < String.length(emphasis_indicator_string)) {
      if (emphasis_indicator_string.[current_position^] != current_tag^) {
        emphasis_tag_strings :=
          List.append(
            emphasis_tag_strings^,
            [
              {
                text: current_tag_string^,
                constituentCharacter: String.make(1, current_tag^),
              },
            ],
          );
        current_tag := emphasis_indicator_string.[current_position^];
        current_tag_string := "";
      };

      current_tag_string :=
        current_tag_string^
        ++ String.sub(emphasis_indicator_string, current_position^, 1);
      current_position := current_position^ + 1;
    };

    emphasis_tag_strings :=
      List.append(
        emphasis_tag_strings^,
        [
          {
            text: current_tag_string^,
            constituentCharacter: String.make(1, current_tag^),
          },
        ],
      );

    emphasis_tag_strings^;
  };

let stack: ref(Stack.t(SpanTypes.t)) = ref(Stack.create());

let current_tag_string: ref(option(emphasis_tag_string)) = ref(None);

let top_emphasis_node_exists: node_type => bool =
  node_type => {
    let exists: ref(bool) = ref(false);

    Stack.iter(
      element =>
        switch (element) {
        | SpanTypes.Node(data) =>
          if (data.nodeType == node_type) {
            exists := true;
          }
        | _ => ()
        },
      stack^,
    );

    exists^;
  };

let get_nodes_from_stack: node_type => list(SpanTypes.t) =
  source => {
    let nodes: ref(list(SpanTypes.t)) = ref([]);
    let ready = ref(false);

    while (! Stack.is_empty(stack^) && ! ready^) {
      let element = Stack.pop(stack^);

      switch (element) {
      | Node(data) =>
        if (data.nodeType == source) {
          nodes := [Node(data), ...nodes^];
          ready := true;
        } else {
          nodes := [SpanTag(TextFragment(data.tagString)), ...nodes^];
        }
      | c => nodes := [c, ...nodes^]
      };
    };

    nodes^;
  };

let match_emphasis_tag_string:
  (list(SpanTypes.t), emphasis_tag_string) => string =
  (nodes, tag_string) => {
    let topmost_node = List.hd(nodes);

    switch (topmost_node) {
    | Node(data) =>
      if (data.tagString == tag_string.text) {
        List.iteri(
          (index, element) =>
            if (index == 0) {
              Stack.push(SpanTag(OpeningEmphasisSpanTag), stack^);
            } else {
              Stack.push(element, stack^);
            },
          nodes,
        );

        let emphasis_tag_length = String.length(tag_string.text);

        if (emphasis_tag_length == 1) {
          Stack.push(
            SpanTag(ClosingEmphasisTag(tag_string.text, EmphaticStress)),
            stack^,
          );
        } else if (emphasis_tag_length == 2) {
          Stack.push(
            SpanTag(ClosingEmphasisTag(tag_string.text, StrongImportance)),
            stack^,
          );
        } else {
          Stack.push(
            SpanTag(
              ClosingEmphasisTag(
                tag_string.text,
                EmphaticStressStrongImportance,
              ),
            ),
            stack^,
          );
        };

        current_tag_string := None;

        "";
      } else if (String.length(data.tagString)
                 < String.length(tag_string.text)) {
        List.iteri(
          (index, element) =>
            if (index == 0) {
              Stack.push(SpanTag(OpeningEmphasisSpanTag), stack^);
            } else {
              Stack.push(element, stack^);
            },
          nodes,
        );

        let emphasis_tag_length = String.length(data.tagString);

        if (emphasis_tag_length == 1) {
          Stack.push(
            SpanTag(ClosingEmphasisTag(data.tagString, EmphaticStress)),
            stack^,
          );
        } else if (emphasis_tag_length == 2) {
          Stack.push(
            SpanTag(ClosingEmphasisTag(data.tagString, StrongImportance)),
            stack^,
          );
        } else {
          Stack.push(
            SpanTag(
              ClosingEmphasisTag(
                data.tagString,
                EmphaticStressStrongImportance,
              ),
            ),
            stack^,
          );
        };

        let remaining_tag_string =
          String.sub(
            tag_string.text,
            String.length(data.tagString),
            String.length(tag_string.text) - String.length(data.tagString),
          );

        current_tag_string :=
          Some({...tag_string, text: remaining_tag_string});

        remaining_tag_string;
      } else if (String.length(data.tagString)
                 > String.length(tag_string.text)) {
        let remaining_tag_string =
          String.sub(
            data.tagString,
            0,
            String.length(data.tagString) - String.length(tag_string.text),
          );

        List.iteri(
          (index, element) =>
            if (index == 0) {
              Stack.push(
                Node({...data, tagString: remaining_tag_string}),
                stack^,
              );
              Stack.push(SpanTag(OpeningEmphasisSpanTag), stack^);
            } else {
              Stack.push(element, stack^);
            },
          nodes,
        );

        let emphasis_tag_length = String.length(tag_string.text);

        if (emphasis_tag_length == 1) {
          Stack.push(
            SpanTag(ClosingEmphasisTag(tag_string.text, EmphaticStress)),
            stack^,
          );
        } else if (emphasis_tag_length == 2) {
          Stack.push(
            SpanTag(ClosingEmphasisTag(tag_string.text, StrongImportance)),
            stack^,
          );
        } else {
          Stack.push(
            SpanTag(
              ClosingEmphasisTag(
                tag_string.text,
                EmphaticStressStrongImportance,
              ),
            ),
            stack^,
          );
        };

        "";
      } else {
        raise(Not_found);
      }
    | _ => raise(Not_found)
    };
  };

let interprete_as_potential_closing_tag: emphasis_indicator_string => int =
  source => {
    let remaining_tags = ref(source.emphasisTagStrings);
    let current_tag: ref(emphasis_tag_string) =
      ref(List.hd(remaining_tags^));

    while (List.length(remaining_tags^) > 0) {
      if (String.length(current_tag^.text) == 0) {
        current_tag := List.hd(remaining_tags^);
      };
      let current_node_type =
        current_tag^.constituentCharacter == "_" ?
          UnderscoreEmphasis : AsteriskEmphasis;
      current_tag_string := Some(current_tag^);

      if (! top_emphasis_node_exists(current_node_type)) {
        Stack.push(SpanTag(TextFragment(current_tag^.text)), stack^);
        current_tag := {...current_tag^, text: ""};
      } else {
        let nodes = get_nodes_from_stack(current_node_type);

        current_tag :=
          {
            ...current_tag^,
            text: match_emphasis_tag_string(nodes, current_tag^),
          };
      };

      if (String.length(current_tag^.text) == 0) {
        remaining_tags := List.tl(remaining_tags^);
      };
    };

    String.length(source.text);
  };

let interprete_emphasis_indicator_string: emphasis_indicator_string => int =
  source =>
    if (source.flanking == NonFlanking) {
      /* Interprete emphasis_indicator string as TextFragment */
      Stack.push(SpanTag(TextFragment(source.text)), stack^);

      String.length(source.text);
    } else if (source.flanking == LeftFlanking) {
      /* Interprete emphasis_indicator string as potential OpeningTag */
      List.iter(
        (tag: emphasis_tag_string) =>
          Stack.push(
            Node({
              tagString: tag.text,
              nodeType:
                tag.constituentCharacter == "_" ?
                  UnderscoreEmphasis : AsteriskEmphasis,
              linkedContentStart: None,
              htmlTagName: None,
            }),
            stack^,
          ),
        source.emphasisTagStrings,
      );

      String.length(source.text);
    } else {
      interprete_as_potential_closing_tag(source);
    };

let identify: (Stack.t(SpanTypes.t), string, int) => int =
  (new_stack, input_character_sequence, current_position) => {
    stack := new_stack;

    let remaining_input =
      String.sub(
        input_character_sequence,
        current_position,
        String.length(input_character_sequence) - current_position,
      );

    let match_emphasis_indicator =
      Js.String.match(Regex.emphasis_indicator, remaining_input);

    switch (match_emphasis_indicator) {
    | None => raise(Not_found)
    | Some(captures) =>
      let emphasis_indicator_string = captures[1];

      let left_fringe_rank =
        get_left_fringe_rank(input_character_sequence, current_position);
      let right_fringe_rank =
        get_right_fringe_rank(input_character_sequence, current_position);
      let flanking = compute_flanking(left_fringe_rank, right_fringe_rank);
      let emphasis_tag_strings =
        get_emphasis_tag_strings(emphasis_indicator_string);

      interprete_emphasis_indicator_string({
        text: emphasis_indicator_string,
        emphasisTagStrings: emphasis_tag_strings,
        flanking,
        rightFringeRank: right_fringe_rank,
        leftFringeRank: left_fringe_rank,
      });
    };
  };
