module type S = {
    let name: string;

    type t;

    type ParserV2.span += Span(t);

    let tryParse: (string, ~spanParser: string => list(ParserV2.span)) => option(t);

    let unparse: (t, ~spanUnparser: list(ParserV2.span) => string) => string;

    let tryDecode: (Js.Json.t, ~spanDecoder: Js.Json.t => option(list(ParserV2.span))) => option(t);

    let encode: (t, ~spanEncoder: list(ParserV2.span) => Js.Json.t) => Js.Json.t;
};