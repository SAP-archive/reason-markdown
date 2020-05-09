module type S = {
    let name: string;

    type t;

    type ParserV2.block += Block(t);

    let tryParse: (string, ~blockParser: string => list(ParserV2.block), ~spanParser: string => list(ParserV2.span)) => option(t);

    let unparse: (t, ~blockUnparser: list(ParserV2.block) => string, ~spanUnparser: list(ParserV2.span) => string) => string;

    let tryDecode: (Js.Json.t, ~blockDecoder: Js.Json.t => option(list(ParserV2.block)), ~spanDecoder: Js.Json.t => option(list(ParserV2.span))) => option(t);

    let encode: (t, ~blockEncoder: list(ParserV2.block) => Js.Json.t, ~spanEncoder: list(ParserV2.span) => Js.Json.t) => Js.Json.t;
};