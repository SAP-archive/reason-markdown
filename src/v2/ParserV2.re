type span = ..;
type block = ..;

module type S = {
    let parse: string => list(block);

    let unparse: list(block) => string;

    let decode: Js.Json.t => option(list(block));

    let encode: list(block) => Js.Json.t;
};