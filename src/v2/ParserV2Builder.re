open ParserV2;

module type S = {
    let parseSpan: (string, ~spanParser: string => list(span)) => span;

    let parseBlock: (string, ~blockParser: string => list(block), ~spanParser: string => list(span)) => block;

    let unparseSpan: (span, ~spanUnparser: list(span) => string) => string;

    let unparseBlock: (block, ~blockUnparser: list(block) => string, ~spanUnparser: list(span) => string) => string;

    let decodeSpan: (Js.Json.t, ~spanDecoder: Js.Json.t => list(span)) => option(span);

    let decodeBlock: (Js.Json.t, ~blockDecoder: Js.Json.t => list(block), ~spanDecoder: Js.Json.t => list(span)) => option(block);

    let encodeSpan: (span, ~spanEncoder: list(span) => Js.Json.t) => Js.Json.t;

    let encodeBlock: (block, ~blockEncoder: list(block) => Js.Json.t, ~spanEncoder: list(span) => Js.Json.t) => Js.Json.t;
};

exception NoSuitableBlockFound;
exception NoSuitableSpanFound;

module Base: S = {
    let parseSpan = (_, ~spanParser as _) => raise(NoSuitableSpanFound);

    let parseBlock = (_, ~blockParser as _, ~spanParser as _) => raise(NoSuitableBlockFound);

    let unparseSpan = (_, ~spanUnparser as _) => raise(NoSuitableSpanFound);

    let unparseBlock = (_, ~blockUnparser as _, ~spanUnparser as _) => raise(NoSuitableBlockFound);

    let decodeSpan = (_, ~spanDecoder as _) => None;

    let decodeBlock = (_, ~blockDecoder as _, ~spanDecoder as _) => None;

    let encodeSpan = (_, ~spanEncoder as _) => raise(NoSuitableSpanFound);

    let encodeBlock = (_, ~blockEncoder as _, ~spanEncoder as _) => raise(NoSuitableBlockFound);
};

module AddSpanParser = (Parser: S, SpanParser: SpanParser.S) => {
   include Parser;

   let parseSpan = (markdown, ~spanParser) => 
        switch (SpanParser.tryParse(markdown, ~spanParser)) {
            | Some(t) => SpanParser.Span(t)
            | _ => parseSpan(markdown, ~spanParser)
        };

    let unparseSpan = (span, ~spanUnparser) =>
        switch (span) {
        | SpanParser.Span(t) => SpanParser.unparse(t, ~spanUnparser)
        | _ => unparseSpan(span, ~spanUnparser);
        };
    
    let decodeSpan = (json, ~spanDecoder) => 
        json
        ->Js.Json.decodeObject
        ->Belt.Option.flatMap(dict => 
            dict
            ->Js.Dict.get("variant")
            ->Belt.Option.flatMap(Js.Json.decodeString)
            ->Belt.Option.flatMap(variant =>
                if (variant == SpanParser.name) {
                    dict
                    ->Js.Dict.get("value")
                    ->Belt.Option.flatMap(SpanParser.tryDecode(_, ~spanDecoder))
                } else {
                    None
                }
            )
        );

    let encodeSpan = (span, ~spanEncoder) =>
        switch (span) {
        | SpanParser.Span(t) => 
            Js.Json.object_(Js.Dict.fromArray([|
                ("variant", Js.Json.string(SpanParser.name)),
                ("value", SpanParser.encode(t, ~spanEncoder))
            |]))
        | _ => encodeSpan(span, ~spanEncoder)
        };
};

module AddBlockParser = (Parser: S, BlockParser: BlockParser.S) => {
    include Parser;

   let parseBlock = (markdown, ~blockParser, ~spanParser) => 
        switch (BlockParser.tryParse(markdown, ~blockParser, ~spanParser)) {
            | Some(t) => BlockParser.Block(t)
            | _ => parseBlock(markdown, ~blockParser, ~spanParser)
        };

    let unparseBlock = (block, ~blockUnparser, ~spanUnparser) =>
        switch (block) {
        | BlockParser.Block(t) => BlockParser.unparse(t, ~blockUnparser, ~spanUnparser)
        | _ => unparseBlock(block, ~blockUnparser, ~spanUnparser);
        };
    
    let decodeBlock = (json, ~blockDecoder, ~spanDecoder) => 
        json
        ->Js.Json.decodeObject
        ->Belt.Option.flatMap(dict => 
            dict
            ->Js.Dict.get("variant")
            ->Belt.Option.flatMap(Js.Json.decodeString)
            ->Belt.Option.flatMap(variant =>
                if (variant == BlockParser.name) {
                    dict
                    ->Js.Dict.get("value")
                    ->Belt.Option.flatMap(BlockParser.tryDecode(_, ~blockDecoder, ~spanDecoder))
                } else {
                    None
                }
            )
        );

    let encodeBlock = (block, ~blockEncoder, ~spanEncoder) =>
        switch (block) {
        | BlockParser.Block(t) => 
            Js.Json.object_(Js.Dict.fromArray([|
                ("variant", Js.Json.string(BlockParser.name)),
                ("value", BlockParser.encode(t, ~blockEncoder, ~spanEncoder))
            |]))
        | _ => encodeBlock(block, ~blockEncoder, ~spanEncoder)
        };
};

module Build = (Parser: S) => {
    include Parser;

    let /*rec*/ _parseSpans = _markdown => {
        /*
         * until no more character call parseSpan(markdown, ~spanParser=parseSpans)
         */
        assert(false);
    };
    
    let /*rec*/ parseBlocks = _markdown => {
        /*
         * until no more lines call parseBlock(markdown, ~blockParser=parseBlocks, ~spanParser=parseSpans)
         */
        assert(false);
    };

    let parse = parseBlocks;

    let /*rec*/ _unparseSpans = _spans => {
        /*
         * until no more spans call unparseSpan(span, ~spanUnparser=unparseSpans)
         */
        assert(false);
    };

    let /*rec*/ unparseBlocks = _blocks => {
        /*
         * until no more blocks call unparseBlock(block, ~blockUnparser=unparseBlock, ~spanUnparser=unparseSpans)
         */
        assert(false);
    };

    let unparse = unparseBlocks;

    let /*rec*/ _decodeSpans = _json => {
        /*
         * until no more json call decodeSpan(json, ~spanDecoder)
         */
        assert(false);
    };

    let /*rec*/ decodeBlocks = _json => {
        /*
         * until no more json call decodeBlock(json, ~blockDecoder, ~spanDecoder)
         */
        assert(false);
    };

    let decode = decodeBlocks;

    let /*rec*/ _encodeSpans = _spans => {
        /*
         * until no more spans call encodeSpan(span, ~spanEncoder)
         */
        assert(false);
    };

    let /*rec*/ encodeBlocks = _blocks => {
        /*
         * until no more blocks call encodeBlock(json, ~blockEncoder, ~spanDecoder)
         */
        assert(false);
    };

    let encode = encodeBlocks;
};