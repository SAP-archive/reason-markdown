module Span = {
  let rec get_url_for_reference_id:
    (list(BlockContext.reference), string) => option((string, string)) =
    (references, id) =>
      switch (references) {
      | [(reference_id, reference_url, reference_title), ...tail] =>
        if (reference_id == id) {
          Some((reference_url, reference_title));
        } else {
          get_url_for_reference_id(tail, id);
        }
      | [] => None
      };

  module Emphasis = {
    let emphasis_variants_markdown: string = "*emphasis* _emphasis_ **strong** __strong__ ___emphasis-strong___ ***emphasis-strong***";
    let emphasis_variants_ast: AST.spans = [
      EmphaticStress("*", [Text("emphasis")]),
      Text(" "),
      EmphaticStress("_", [Text("emphasis")]),
      Text(" "),
      StrongImportance("**", [Text("strong")]),
      Text(" "),
      StrongImportance("__", [Text("strong")]),
      Text(" "),
      EmphaticStressStrongImportance("___", [Text("emphasis-strong")]),
      Text(" "),
      EmphaticStressStrongImportance("***", [Text("emphasis-strong")]),
    ];

    let emphasis_flanking_markdown: string = "*emphasis*non-flanking*emphasis* _empha*sis_";
    let emphasis_flanking_ast: AST.spans = [
      EmphaticStress("*", [Text("emphasis*non-flanking*emphasis")]),
      Text(" "),
      EmphaticStress("_", [Text("empha*sis")]),
    ];

    let emphasis_interlaced_markdown: string = "*emphasis __strong__ emphasis* _emphasis **strong**_ __strong **strong** strong__ *emphasis _emphasis_ emphasis _emphasis_ emphasis*";
    let emphasis_interlaced_ast: AST.spans = [
      EmphaticStress(
        "*",
        [
          Text("emphasis "),
          StrongImportance("__", [Text("strong")]),
          Text(" emphasis"),
        ],
      ),
      Text(" "),
      EmphaticStress(
        "_",
        [Text("emphasis "), StrongImportance("**", [Text("strong")])],
      ),
      Text(" "),
      StrongImportance(
        "__",
        [
          Text("strong "),
          StrongImportance("**", [Text("strong")]),
          Text(" strong"),
        ],
      ),
      Text(" "),
      EmphaticStress(
        "*",
        [
          Text("emphasis "),
          EmphaticStress("_", [Text("emphasis")]),
          Text(" emphasis "),
          EmphaticStress("_", [Text("emphasis")]),
          Text(" emphasis"),
        ],
      ),
    ];
  };

  module Code = {
    let code_variants_markdown: string = "`simple` ``advanced` with inner backtick`` ";
    let code_variants_ast: AST.spans = [
      Code("simple"),
      Text(" "),
      Code("advanced` with inner backtick"),
      Text(" "),
    ];
  };

  module Link = {
    let link_variants_markdown: string = "[I'm a link][withrefid] [I'm also a link](without/angle/brackets) [Number three](<with/angle/brackets>)";
    let link_variants_ast: AST.spans = [
      Link({title: [Text("I'm a link")], url: None}),
      Text(" "),
      Link({
        title: [Text("I'm also a link")],
        url: Some("without/angle/brackets"),
      }),
      Text(" "),
      Link({
        title: [Text("Number three")],
        url: Some("with/angle/brackets"),
      }),
    ];

    let build_link_with_references:
      (list(BlockContext.reference), AST.spans, string) => AST.link_data =
      (references, title_spans, reference_id) => {
        let maybe_reference =
          get_url_for_reference_id(references, reference_id);

        switch (maybe_reference) {
        | Some((url, _title)) => {title: title_spans, url: Some(url)}
        | None => {title: title_spans, url: None}
        };
      };

    let link_with_references_markdown: string = "[With references][id_one] [With references][id_two]";
    let link_with_references_ast: list(BlockContext.reference) => AST.spans =
      references => [
        Link(
          build_link_with_references(
            references,
            [Text("With references")],
            "id_one",
          ),
        ),
        Text(" "),
        Link(
          build_link_with_references(
            references,
            [Text("With references")],
            "id_two",
          ),
        ),
      ];
  };

  module Image = {
    let image_variants_markdown: string = "![one][reference] ![two](without/brackets) ![three](<with/brackets>) ![four](url \"with title\")";
    let image_variants_ast: AST.spans = [
      Image({alternativeText: "one", url: None, title: None}),
      Text(" "),
      Image({
        alternativeText: "two",
        url: Some("without/brackets"),
        title: None,
      }),
      Text(" "),
      Image({
        alternativeText: "three",
        url: Some("with/brackets"),
        title: None,
      }),
      Text(" "),
      Image({
        alternativeText: "four",
        url: Some("url"),
        title: Some("with title"),
      }),
    ];

    let build_image_with_references:
      (list(BlockContext.reference), string, string) => AST.image_data =
      (references, altText, reference_id) => {
        let maybe_reference =
          get_url_for_reference_id(references, reference_id);

        switch (maybe_reference) {
        | Some((url, title)) => {
            alternativeText: altText,
            title: Some(title),
            url: Some(url),
          }
        | None => {alternativeText: altText, title: None, url: None}
        };
      };

    let image_with_references_markdown: string = "![one][id_one] ![two][id_two]";
    let image_with_references_ast: list(BlockContext.reference) => AST.spans =
      references => [
        Image(build_image_with_references(references, "one", "id_one")),
        Text(" "),
        Image(build_image_with_references(references, "two", "id_two")),
      ];
  };

  let spans_interlaced_markdown: string = "[*formatted text* in __link__](my/url) [![image in url](image/url)](link/url) `code reads over *emphasis*` ![no *styling* in image](url) *emphased `code`*";
  let spans_interlaced_ast: AST.spans = [
    Link({
      title: [
        EmphaticStress("*", [Text("formatted text")]),
        Text(" in "),
        StrongImportance("__", [Text("link")]),
      ],
      url: Some("my/url"),
    }),
    Text(" "),
    Link({
      title: [
        Image({
          alternativeText: "image in url",
          title: None,
          url: Some("image/url"),
        }),
      ],
      url: Some("link/url"),
    }),
    Text(" "),
    Code("code reads over *emphasis*"),
    Text(" "),
    Image({
      alternativeText: "no *styling* in image",
      title: None,
      url: Some("url"),
    }),
    Text(" "),
    EmphaticStress("*", [Text("emphased "), Code("code")]),
  ];
};

module Block = {
  module AtxStyleHeader = {
    let variants_markdown: string = {js|# header
# header closed #
# header closed ##|js};
    let variants_blocks: list(BlockTypes.t) = [
      {blockType: AtxStyleHeader({line: "# header"}), lines: ["# header"]},
      {
        blockType: AtxStyleHeader({line: "# header closed #"}),
        lines: ["# header closed #"],
      },
      {
        blockType: AtxStyleHeader({line: "# header closed ##"}),
        lines: ["# header closed ##"],
      },
    ];
  };

  module CodeBlock = {
    let variants_markdown: string = {js|
    I'm
    a
    code block|js};
    let variants_blocks: list(BlockTypes.t) = [
      {blockType: Null, lines: [""]},
      {
        blockType: Code({lines: ["    I'm", "    a", "    code block"]}),
        lines: ["    I'm", "    a", "    code block"],
      },
    ];
  };

  module HorizontalRule = {
    let variants_markdown: string = {js|***

---

___|js};
    let variants_blocks: list(BlockTypes.t) = [
      {blockType: HorizontalRule(), lines: ["***"]},
      {blockType: Null, lines: [""]},
      {blockType: HorizontalRule(), lines: ["---"]},
      {blockType: Null, lines: [""]},
      {blockType: HorizontalRule(), lines: ["___"]},
    ];
  };

  module OrderedList = {
    let variants_markdown: string = {js|1. first
2. second


1. first
  1. sub
  2. sub|js};
    let variants_blocks: list(BlockTypes.t) = [
      {
        blockType:
          OrderedList({
            starterPattern: "1. ",
            lines: ["1. first", "2. second"],
          }),
        lines: ["1. first", "2. second"],
      },
      {blockType: Null, lines: [""]},
      {blockType: Null, lines: [""]},
      {
        blockType:
          OrderedList({
            starterPattern: "1. ",
            lines: ["1. first", "  1. sub", "  2. sub"],
          }),
        lines: ["1. first", "  1. sub", "  2. sub"],
      },
    ];
  };

  module Paragraph = {
    let variants_markdown: string = {js|I'm a plain
old paragraph


I'm another plain
old paragraph|js};
    let variants_blocks: list(BlockTypes.t) = [
      {
        blockType: Paragraph({lines: ["I'm a plain", "old paragraph"]}),
        lines: ["I'm a plain", "old paragraph"],
      },
      {blockType: Null, lines: [""]},
      {blockType: Null, lines: [""]},
      {
        blockType: Paragraph({lines: ["I'm another plain", "old paragraph"]}),
        lines: ["I'm another plain", "old paragraph"],
      },
    ];
  };

  module Quote = {
    let variants_markdown: string = {js|> Quote|js};
    let variants_blocks: list(BlockTypes.t) = [
      {
        blockType: Quote({lines: ["> Quote"]}),
        lines: ["> Quote"],
      },
    ];
  };

  module ReferenceResolution = {
    let variants_markdown: string = {js|[reference id]: url |js};
    let variants_blocks: list(BlockTypes.t) = [
      {
        blockType:
          ReferenceResolution({
            unprocessedReferenceIdString: "reference id",
            unprocessedUrlString: "url",
            refDefinitionTrailingSequence: " ",
          }),
        lines: ["[reference id]: url "],
      },
    ];
  };

  module SetextStyleHeader = {
    let variants_markdown: string = {js|Header
---
Header
===|js};
    let variants_blocks: list(BlockTypes.t) = [
      {
        blockType: SetextStyleHeader({text: "Header", secondLine: "---"}),
        lines: ["Header", "---"],
      },
      {
        blockType: SetextStyleHeader({text: "Header", secondLine: "==="}),
        lines: ["Header", "==="],
      },
    ];
  };

  module UnorderedList = {
    let variants_markdown: string = {js|- first
- second


+ first
  - sub
  - sub


* first
  * sub
  * sub|js};
    let variants_blocks: list(BlockTypes.t) = [
      {
        blockType:
          UnorderedList({
            starterPattern: "- ",
            lines: ["- first", "- second"],
          }),
        lines: ["- first", "- second"],
      },
      {blockType: Null, lines: [""]},
      {blockType: Null, lines: [""]},
      {
        blockType:
          UnorderedList({
            starterPattern: "+ ",
            lines: ["+ first", "  - sub", "  - sub"],
          }),
        lines: ["+ first", "  - sub", "  - sub"],
      },
      {blockType: Null, lines: [""]},
      {blockType: Null, lines: [""]},
      {
        blockType:
          UnorderedList({
            starterPattern: "* ",
            lines: ["* first", "  * sub", "  * sub"],
          }),
        lines: ["* first", "  * sub", "  * sub"],
      },
    ];
  };

  let variants_markdown: string = {js|__Advertisement :)__
You will like those projects!

# h1 Heading 8-)
## h2 Heading

___

---

***

> Blockquotes can also be nested...
>> ...by using additional greater-than signs right next to each other...
> > > ...or with spaces between arrows.

+ Create a list by starting a line with `noice`
+ Sub-lists are made by indenting 2 spaces:
+ Marker character change forces new list start:
  + Ac tristique libero volutpat at
  + Facilisis in pretium nisl aliquet
  + Nulla volutpat aliquam velit
+ Very easy!

1. Lorem ipsum dolor sit amet
2. Consectetur adipiscing elit
3. Integer molestie lorem at massa

[ref id]:(here/i/am) hi|js};
  let variants_blocks: list(BlockTypes.t) = [
    {
      blockType:
        Paragraph({
          lines: ["__Advertisement :)__", "You will like those projects!"],
        }),
      lines: ["__Advertisement :)__", "You will like those projects!"],
    },
    {blockType: Null, lines: [""]},
    {
      blockType: AtxStyleHeader({line: "# h1 Heading 8-)"}),
      lines: ["# h1 Heading 8-)"],
    },
    {
      blockType: AtxStyleHeader({line: "## h2 Heading"}),
      lines: ["## h2 Heading"],
    },
    {blockType: Null, lines: [""]},
    {blockType: HorizontalRule(), lines: ["___"]},
    {blockType: Null, lines: [""]},
    {blockType: HorizontalRule(), lines: ["---"]},
    {blockType: Null, lines: [""]},
    {blockType: HorizontalRule(), lines: ["***"]},
    {blockType: Null, lines: [""]},
    {
      blockType:
        Quote({
          lines: [
            "> Blockquotes can also be nested...",
            ">> ...by using additional greater-than signs right next to each other...",
            "> > > ...or with spaces between arrows.",
          ],
        }),
      lines: [
        "> Blockquotes can also be nested...",
        ">> ...by using additional greater-than signs right next to each other...",
        "> > > ...or with spaces between arrows.",
      ],
    },
    {blockType: Null, lines: [""]},
    {
      blockType:
        UnorderedList({
          starterPattern: "+ ",
          lines: [
            "+ Create a list by starting a line with `noice`",
            "+ Sub-lists are made by indenting 2 spaces:",
            "+ Marker character change forces new list start:",
            "  + Ac tristique libero volutpat at",
            "  + Facilisis in pretium nisl aliquet",
            "  + Nulla volutpat aliquam velit",
            "+ Very easy!",
          ],
        }),
      lines: [
        "+ Create a list by starting a line with `noice`",
        "+ Sub-lists are made by indenting 2 spaces:",
        "+ Marker character change forces new list start:",
        "  + Ac tristique libero volutpat at",
        "  + Facilisis in pretium nisl aliquet",
        "  + Nulla volutpat aliquam velit",
        "+ Very easy!",
      ],
    },
    {blockType: Null, lines: [""]},
    {
      blockType:
        OrderedList({
          starterPattern: "1. ",
          lines: [
            "1. Lorem ipsum dolor sit amet",
            "2. Consectetur adipiscing elit",
            "3. Integer molestie lorem at massa",
          ],
        }),
      lines: [
        "1. Lorem ipsum dolor sit amet",
        "2. Consectetur adipiscing elit",
        "3. Integer molestie lorem at massa",
      ],
    },
    {blockType: Null, lines: [""]},
    {
      blockType:
        ReferenceResolution({
          unprocessedReferenceIdString: "ref id",
          unprocessedUrlString: "(here/i/am)",
          refDefinitionTrailingSequence: " hi",
        }),
      lines: ["[ref id]:(here/i/am) hi"],
    },
  ];
};

module Parser = {
  let simple_markdown: string = {js|
__Advertisement :)__  
You will like those projects!

# h1 Heading 8-)
## h2 Heading

___

---

***

    func main(args) {
      printf(args);
    }

> Blockquotes can also be nested...
>> ...by using additional greater-than signs right next to each other...
> > > ...or with spaces between arrows.

+ Create a list by starting a line with `code`
+ Sub-lists are made by indenting 2 spaces:
  + Ac tristique libero volutpat at
  + Facilisis in pretium nisl aliquet
  + Nulla volutpat aliquam velit
* Marker character change forces new list start:
* Very easy!

1. Lorem ipsum dolor sit amet  
hi
2. Consectetur adipiscing elit
    1. Really
    2. nice
3. Integer molestie lorem at massa

[ref id]: here/i/am Here i am
|js};
  let simple_ast: AST.t = [
    Paragraph([
      StrongImportance("__", [Text("Advertisement :)")]),
      LineBreak,
      Text("You will like those projects!"),
    ]),
    Heading(1, [Text("h1 Heading 8-)")]),
    Heading(2, [Text("h2 Heading")]),
    HorizontalRule,
    HorizontalRule,
    HorizontalRule,
    Code("func main(args) {\n  printf(args);\n}"),
    Quote([
      Paragraph([Text(" Blockquotes can also be nested...")]),
      Paragraph([
        Text(
          "> ...by using additional greater-than signs right next to each other...",
        ),
      ]),
      Paragraph([Text(" > > ...or with spaces between arrows.")]),
    ]),
    UnorderedList([
      [
        Paragraph([
          Text("Create a list by starting a line with "),
          Code("code")
        ]),
      ],
      [
        Paragraph([Text("Sub-lists are made by indenting 2 spaces:")]),
        UnorderedList([
          [Paragraph([Text("Ac tristique libero volutpat at")])],
          [Paragraph([Text("Facilisis in pretium nisl aliquet")])],
          [Paragraph([Text("Nulla volutpat aliquam velit")])],
        ]),
      ],
    ]),
    UnorderedList([
      [
        Paragraph([Text("Marker character change forces new list start:")]),
      ],
      [Paragraph([Text("Very easy!")])],
    ]),
    OrderedList([
      {
        number: 1,
        blocks: [Paragraph([Text("Lorem ipsum dolor sit amet"), LineBreak, Text("hi")])],
      },
      {
        number: 2,
        blocks: [
          Paragraph([Text("Consectetur adipiscing elit")]),
          OrderedList([
            {number: 1, blocks: [Paragraph([Text("Really")])]},
            {number: 2, blocks: [Paragraph([Text("nice")])]},
          ]),
        ],
      },
      {
        number: 3,
        blocks: [Paragraph([Text("Integer molestie lorem at massa")])],
      },
    ]),
    ReferenceResolution({
      referenceId: "ref id",
      linkUrl: "here/i/am",
      linkTitle: "",
    }),
  ];
};
