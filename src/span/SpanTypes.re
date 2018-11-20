/* type declarations used for internal and external representations */

type node_type =
  | AsteriskEmphasis
  | UnderscoreEmphasis
  | Link
  | RawHtml;

type node = {
  tagString: string,
  nodeType: node_type,
  linkedContentStart: option(int),
  htmlTagName: option(string),
};

type link_data = {
  title: option(string),
  url: option(string),
  referenceId: option(string),
};

type span_type =
  | Emphasis
  | Code
  | Link;

type emphasis_type =
  | EmphaticStress
  | StrongImportance
  | EmphaticStressStrongImportance;

type span_data = {spanType: span_type};

type text_data = string;

type image_data = {
  altText: string,
  title: option(string),
  referenceId: option(string),
  url: option(string),
};

type span =
  | OpeningEmphasisSpanTag
  | OpeningCodeSpanTag
  | OpeningLinkSpanTag
  | ClosingEmphasisTag(string, emphasis_type)
  | ClosingCodeTag
  | ClosingLinkTag(link_data)
  | SelfContainedSpanTag(span_data)
  | ImageSpanTag(image_data)
  | TextFragment(string)
  | LineBreak;

type t =
  | SpanTag(span)
  | Node(node);
