open Jest;

let () =
  describe(
    "Span",
    ExpectJs.(
      () => {
        test("emphasis variants", () =>
          expect(TestUtil.Span.Emphasis.emphasis_variants_ast)
          |> toEqual(
               Span.get_ast(
                 Span.identify(
                   [TestUtil.Span.Emphasis.emphasis_variants_markdown],
                 ),
                 {references: []},
               ),
             )
        );
        test("emphasis flanking", () =>
          expect(TestUtil.Span.Emphasis.emphasis_flanking_ast)
          |> toEqual(
               Span.get_ast(
                 Span.identify(
                   [TestUtil.Span.Emphasis.emphasis_flanking_markdown],
                 ),
                 {references: []},
               ),
             )
        );
        test("emphasis interlacing", () =>
          expect(TestUtil.Span.Emphasis.emphasis_interlaced_ast)
          |> toEqual(
               Span.get_ast(
                 Span.identify(
                   [TestUtil.Span.Emphasis.emphasis_interlaced_markdown],
                 ),
                 {references: []},
               ),
             )
        );
      }
    ),
  );

let () =
  describe(
    "Span",
    ExpectJs.(
      () =>
        test("code variants", () =>
          expect(TestUtil.Span.Code.code_variants_ast)
          |> toEqual(
               Span.get_ast(
                 Span.identify([TestUtil.Span.Code.code_variants_markdown]),
                 {references: []},
               ),
             )
        )
    ),
  );

let () =
  describe(
    "Span",
    ExpectJs.(
      () => {
        test("link", () =>
          expect(TestUtil.Span.Link.link_variants_ast)
          |> toEqual(
               Span.get_ast(
                 Span.identify([TestUtil.Span.Link.link_variants_markdown]),
                 {references: []},
               ),
             )
        );
        test("link references", () => {
          let references: list(BlockContext.reference) = [
            ("id_one", "first/url", "first/title"),
            ("id_two", "second/url", "second/title"),
          ];

          expect(TestUtil.Span.Link.link_with_references_ast(references))
          |> toEqual(
               Span.get_ast(
                 Span.identify(
                   [TestUtil.Span.Link.link_with_references_markdown],
                 ),
                 {references: references},
               ),
             );
        });
      }
    ),
  );

let () =
  describe(
    "Span",
    ExpectJs.(
      () => {
        test("image variants", () =>
          expect(TestUtil.Span.Image.image_variants_ast)
          |> toEqual(
               Span.get_ast(
                 Span.identify([TestUtil.Span.Image.image_variants_markdown]),
                 {references: []},
               ),
             )
        );
        test("image references", () => {
          let references: list(BlockContext.reference) = [
            ("id_one", "first/url", "first/title"),
            ("id_two", "second/url", "second/title"),
          ];

          expect(TestUtil.Span.Image.image_with_references_ast(references))
          |> toEqual(
               Span.get_ast(
                 Span.identify(
                   [TestUtil.Span.Image.image_with_references_markdown],
                 ),
                 {references: references},
               ),
             );
        });
      }
    ),
  );

let () =
  describe(
    "Span",
    ExpectJs.(
      () =>
        test("all", () =>
          expect(TestUtil.Span.spans_interlaced_ast)
          |> toEqual(
               Span.get_ast(
                 Span.identify([TestUtil.Span.spans_interlaced_markdown]),
                 {references: []},
               ),
             )
        )
    ),
  );

let () =
  describe(
    "Block",
    ExpectJs.(
      () =>
        test("AtxStyleHeader", () => {
          let (blocks, _context) =
            MarkdownBlock.identify_blocks(
              Util.prepare(TestUtil.Block.AtxStyleHeader.variants_markdown),
            );

          expect(TestUtil.Block.AtxStyleHeader.variants_blocks)
          |> toEqual(blocks);
        })
    ),
  );

let () =
  describe(
    "Block",
    ExpectJs.(
      () =>
        test("CodeBlock", () => {
          let (blocks, _context) =
            MarkdownBlock.identify_blocks(
              Util.prepare(TestUtil.Block.CodeBlock.variants_markdown),
            );

          expect(TestUtil.Block.CodeBlock.variants_blocks) |> toEqual(blocks);
        })
    ),
  );

let () =
  describe(
    "Block",
    ExpectJs.(
      () =>
        test("HorizontalRule", () => {
          let (blocks, _context) =
            MarkdownBlock.identify_blocks(
              Util.prepare(TestUtil.Block.HorizontalRule.variants_markdown),
            );

          expect(TestUtil.Block.HorizontalRule.variants_blocks)
          |> toEqual(blocks);
        })
    ),
  );

let () =
  describe(
    "Block",
    ExpectJs.(
      () =>
        test("OrderedList", () => {
          let (blocks, _context) =
            MarkdownBlock.identify_blocks(
              Util.prepare(TestUtil.Block.OrderedList.variants_markdown),
            );

          expect(TestUtil.Block.OrderedList.variants_blocks)
          |> toEqual(blocks);
        })
    ),
  );

let () =
  describe(
    "Block",
    ExpectJs.(
      () =>
        test("Paragraph", () => {
          let (blocks, _context) =
            MarkdownBlock.identify_blocks(
              Util.prepare(TestUtil.Block.Paragraph.variants_markdown),
            );

          expect(TestUtil.Block.Paragraph.variants_blocks) |> toEqual(blocks);
        })
    ),
  );

let () =
  describe(
    "Block",
    ExpectJs.(
      () =>
        test("Quote", () => {
          let (blocks, _context) =
            MarkdownBlock.identify_blocks(
              Util.prepare(TestUtil.Block.Quote.variants_markdown),
            );

          expect(TestUtil.Block.Quote.variants_blocks) |> toEqual(blocks);
        })
    ),
  );

let () =
  describe(
    "Block",
    ExpectJs.(
      () =>
        test("ReferenceResolution", () => {
          let (blocks, _context) =
            MarkdownBlock.identify_blocks(
              Util.prepare(
                TestUtil.Block.ReferenceResolution.variants_markdown,
              ),
            );

          expect(TestUtil.Block.ReferenceResolution.variants_blocks)
          |> toEqual(blocks);
        })
    ),
  );

let () =
  describe(
    "Block",
    ExpectJs.(
      () =>
        test("SetextStyleHeader", () => {
          let (blocks, _context) =
            MarkdownBlock.identify_blocks(
              Util.prepare(
                TestUtil.Block.SetextStyleHeader.variants_markdown,
              ),
            );

          expect(TestUtil.Block.SetextStyleHeader.variants_blocks)
          |> toEqual(blocks);
        })
    ),
  );

let () =
  describe(
    "Block",
    ExpectJs.(
      () =>
        test("UnorderedList", () => {
          let (blocks, _context) =
            MarkdownBlock.identify_blocks(
              Util.prepare(TestUtil.Block.UnorderedList.variants_markdown),
            );

          expect(TestUtil.Block.UnorderedList.variants_blocks)
          |> toEqual(blocks);
        })
    ),
  );

let () =
  describe(
    "Parser",
    ExpectJs.(
      () => {
        test("simple markdown to ast", () =>
          expect(TestUtil.Parser.simple_ast)
          |> toEqual(Parser.markdown_to_ast(TestUtil.Parser.simple_markdown))
        );
        test("simple ast to markdown", () => {
          let markdown = Parser.ast_to_markdown(TestUtil.Parser.simple_ast);

          expect(TestUtil.Parser.simple_ast)
          |> toEqual(Parser.markdown_to_ast(markdown));
        });
      }
    ),
  );
