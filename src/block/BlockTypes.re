type reference_resolution = {
  unprocessedReferenceIdString: string,
  unprocessedUrlString: string,
  refDefinitionTrailingSequence: string,
};

type setext_style_header = {
  text: string,
  secondLine: string,
};

type code = {lines: list(string)};

type atx_style_header = {line: string};

type quote = {lines: list(string)};

type horizontal_rule = unit;

type unordered_list = {
  starterPattern: string,
  lines: list(string),
};

type ordered_list = {
  starterPattern: string,
  lines: list(string),
};

type ordered_list_item = {
  number: int,
  lines: list(string),
};

type unordered_list_item = list(string);

type paragraph = {lines: list(string)};

type type_ =
  | UnorderedList(unordered_list)
  | OrderedList(ordered_list)
  | HorizontalRule(horizontal_rule)
  | ReferenceResolution(reference_resolution)
  | SetextStyleHeader(setext_style_header)
  | Code(code)
  | AtxStyleHeader(atx_style_header)
  | Quote(quote)
  | Paragraph(paragraph)
  | Null;

type t = {
  blockType: type_,
  lines: list(string),
};
