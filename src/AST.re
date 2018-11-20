type text = string;

type image_data = {
  alternativeText: text,
  title: option(string),
  url: option(string),
};

type span =
  | LineBreak
  | Text(text)
  | Code(text)
  | Image(image_data)
  | Link(link_data)
  | EmphaticStress(string, list(span))
  | StrongImportance(string, list(span))
  | EmphaticStressStrongImportance(string, list(span))
and link_data = {
  title: list(span),
  url: option(string),
};

type spans = list(span);

type reference_resolution_data = {
  referenceId: string,
  linkUrl: string,
  linkTitle: string,
};

type block =
  | ReferenceResolution(reference_resolution_data)
  | UnorderedList(list(unordered_list_item))
  | OrderedList(list(ordered_list_item))
  | HorizontalRule
  | Code(text)
  | Heading(int, spans)
  | Paragraph(spans)
  | Quote(list(block))
  | Null
and unordered_list_item = list(block)
and ordered_list_item = {
  number: int,
  blocks: list(block),
};

type t = list(block);
