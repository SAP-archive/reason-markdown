module type S = {
    let parseSpan: (string, ~spanParser: string => list(ParserV2.span)) => ParserV2.span;

    let parseBlock: (string, ~blockParser: string => list(ParserV2.block), ~spanParser: string => list(ParserV2.span)) => ParserV2.block;

    let unparseSpan: (ParserV2.span, ~spanUnparser: list(ParserV2.span) => string) => string;

    let unparseBlock: (ParserV2.block, ~blockUnparser: list(ParserV2.block) => string, ~spanUnparser: list(ParserV2.span) => string) => string;

    let decodeSpan: (string, Js.Json.t, ~spanDecoder: Js.Json.t => option(list(ParserV2.span))) => option(ParserV2.span);

    let decodeBlock: (string, Js.Json.t, ~blockDecoder: Js.Json.t => option(list(ParserV2.block)), ~spanDecoder: Js.Json.t => option(list(ParserV2.span))) => option(ParserV2.block);

    let encodeSpan: (ParserV2.span, ~spanEncoder: list(ParserV2.span) => Js.Json.t) => (string, Js.Json.t);

    let encodeBlock: (ParserV2.block, ~blockEncoder: list(ParserV2.block) => Js.Json.t, ~spanEncoder: list(ParserV2.span) => Js.Json.t) => (string, Js.Json.t);
};

exception NoSuitableBlockFound;
exception NoSuitableSpanFound;

module Base: S = {
    let parseSpan = (_, ~spanParser as _) => raise(NoSuitableSpanFound);

    let parseBlock = (_, ~blockParser as _, ~spanParser as _) => raise(NoSuitableBlockFound);

    let unparseSpan = (_, ~spanUnparser as _) => raise(NoSuitableSpanFound);

    let unparseBlock = (_, ~blockUnparser as _, ~spanUnparser as _) => raise(NoSuitableBlockFound);

    let decodeSpan = (_, _, ~spanDecoder as _) => None;

    let decodeBlock = (_, _, ~blockDecoder as _, ~spanDecoder as _) => None;

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
    
    let decodeSpan = (variantTag, json, ~spanDecoder) => 
        if (variantTag == SpanParser.name) {
            SpanParser.tryDecode(json, ~spanDecoder)
            ->Belt.Option.map(t => SpanParser.Span(t));
        } else {
            decodeSpan(variantTag, json, ~spanDecoder);
        };

    let encodeSpan = (span, ~spanEncoder) =>
        switch (span) {
        | SpanParser.Span(t) => (SpanParser.name, SpanParser.encode(t, ~spanEncoder))
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
    
    let decodeBlock = (variantTag, json, ~blockDecoder, ~spanDecoder) => 
        if (variantTag == BlockParser.name) {
            BlockParser.tryDecode(json, ~blockDecoder, ~spanDecoder)
            ->Belt.Option.map(t => BlockParser.Block(t));
        } else {
            decodeBlock(variantTag, json, ~blockDecoder, ~spanDecoder);
        };

    let encodeBlock = (block, ~blockEncoder, ~spanEncoder) =>
        switch (block) {
        | BlockParser.Block(t) => (BlockParser.name, BlockParser.encode(t, ~blockEncoder, ~spanEncoder))
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

    let variantTagAndValueFromJson = json =>
        json
        ->Js.Json.decodeObject
        ->Belt.Option.flatMap(dict => 
            dict
            ->Js.Dict.get("variant")
            ->Belt.Option.flatMap(Js.Json.decodeString)
            ->Belt.Option.flatMap(variantTag => 
                dict
                ->Js.Dict.get("value")
                ->Belt.Option.map(value => (variantTag, value))
            )
        );

    let variantTagAndValueToJson = (variantTag, value) =>
        Js.Json.object_(Js.Dict.fromArray([|
            ("variant", Js.Json.string(variantTag)),
            ("value", value)
        |]));

    let optionTraverseArray = (elements, f) =>
        elements
        ->Belt.Array.reduce(Some([||]), (traversed, element) => 
            traversed
            ->Belt.Option.flatMap(elements => 
                element
                ->f
                ->Belt.Option.map(element => Belt.Array.concat(elements, [|element|]))
            )
        );

    let decodeSpanObject = (json, spanDecoder) => 
        variantTagAndValueFromJson(json)
        ->Belt.Option.flatMap(((variantTag, value)) => decodeSpan(variantTag, value, ~spanDecoder));

    let rec decodeSpans = json => 
        json
        ->Js.Json.decodeArray
        ->Belt.Option.flatMap(optionTraverseArray(_, decodeSpanObject(_, decodeSpans)))
        ->Belt.Option.map(Belt.List.fromArray);

    let decodeBlockObject = (json, blockDecoder, spanDecoder) =>
        variantTagAndValueFromJson(json)
        ->Belt.Option.flatMap(((variantTag, value)) => decodeBlock(variantTag, value, ~blockDecoder, ~spanDecoder));

    let rec decodeBlocks = json => 
        json
        ->Js.Json.decodeArray
        ->Belt.Option.flatMap(optionTraverseArray(_, decodeBlockObject(_, decodeBlocks, decodeSpans)))
        ->Belt.Option.map(Belt.List.fromArray);

    let decode = decodeBlocks;

    let rec encodeSpans = spans =>
        spans
        ->Belt.List.map(json => {
            let (variantTag, value) = encodeSpan(json, ~spanEncoder=encodeSpans);

            variantTagAndValueToJson(variantTag, value);
        })
        ->Belt.List.toArray
        ->Js.Json.array;

    let rec encodeBlocks = blocks =>
        blocks
        ->Belt.List.map(json => {
            let (variantTag, value) = encodeBlock(json, ~blockEncoder=encodeBlocks, ~spanEncoder=encodeSpans);

            variantTagAndValueToJson(variantTag, value);
        })
        ->Belt.List.toArray
        ->Js.Json.array;

    let encode = encodeBlocks;
};