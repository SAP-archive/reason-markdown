module type S = {
    let parseSpan: (string, ~spanParser: string => list(ParserV2.span)) => ParserV2.span;

    let parseBlock: (string, ~blockParser: string => list(ParserV2.block), ~spanParser: string => list(ParserV2.span)) => ParserV2.block;

    let unparseSpan: (ParserV2.span, ~spanUnparser: list(ParserV2.span) => string) => string;

    let unparseBlock: (ParserV2.block, ~blockUnparser: list(ParserV2.block) => string, ~spanUnparser: list(ParserV2.span) => string) => string;

    let decodeSpan: (Js.Json.t, ~spanDecoder: Js.Json.t => list(ParserV2.span)) => option(ParserV2.span);

    let decodeBlock: (Js.Json.t, ~blockDecoder: Js.Json.t => list(ParserV2.block), ~spanDecoder: Js.Json.t => list(ParserV2.span)) => option(ParserV2.block);

    let encodeSpan: (ParserV2.span, ~spanEncoder: list(ParserV2.span) => Js.Json.t) => Js.Json.t;

    let encodeBlock: (ParserV2.block, ~blockEncoder: list(ParserV2.block) => Js.Json.t, ~spanEncoder: list(ParserV2.span) => Js.Json.t) => Js.Json.t;
};

exception NoSuitableBlockFound;
exception NoSuitableSpanFound;

module Base: S;

module AddSpanParser: (Parser: S, SpanParser: SpanParser.S) => S;

module AddBlockParser: (Parser: S, BlockParser: BlockParser.S) => S;

module Build: (Parser: S) => ParserV2.S;