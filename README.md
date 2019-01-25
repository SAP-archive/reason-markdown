# Markdown parser for REASON

## Description

This repository contains a parser for markdown documents (provided in [vanilla-flavored-markdown-syntax](http://www.vfmd.org/)) written in [Reason](https://reasonml.github.io/), which produces an abstract syntax tree (AST). The parser provides two functions:

* Parsing a markdown string to an AST
* Transforming an AST back to a markdown string

The AST can then get interpreted by your source code (for example: Transforming it into HTML for a markdown-editor preview). The project was originally initiated to provide markdown support in a [ReasonReact](https://reasonml.github.io/reason-react/) web app, which is why the project is compiled to JavaScript using the [BuckleScript-Compiler](https://bucklescript.github.io/).

## Requirements

This project doesn't depend on any third-party library.  No downloads are required.

## Installation

Add the repository to your `dependencies` in your `package.json`
```json
"dependencies" : {
  "@sap/reason-markdown": "git+https://github.com/SAP/reason-markdown.git"
}
```
For `Bucklescript` projects add `@sap/reason-markdown` to `bs-dependencies` in your `bs-config.json`
```json
"bs-dependencies": [
  "@sap/reason-markdown"
]
```

## Known Issues

This is a work-in-progress project.  

* Missing `Markdown` syntax elements (To see a list of the available elements click [here](#supported-syntax-elements))

## How to obtain support

Please ask questions or report bugs via [GitHub issues](https://github.com/SAP/reason-markdown/issues)

## Contributing

We welcome external contributions.  Feel free to open a Pull Request with any suggestion and add a description of the bugs you solved/the features you added.

## To-Do

- [ ] Add missing `Markdown` syntax elements (To see a list of the available elements click [here](#supported-syntax-elements))
- [ ] Make the parser modular, so one can include/exclude syntax elements
- [ ] Make the parser extendable

## Version

The current version of the vfmd parser is 0.1.0.


## Usage

### Supported syntax elements

Currently supported elements (vfmd syntax guide: [guide](http://www.vfmd.org/vfmd-spec/syntax/)):
- [ ] Block
    - [x] Atx-Style-Headers
    - [x] Setext-Style-Headers
    - [x] Quotes
    - [ ] Nested quotes
    - [x] Unordered lists
    - [x] Ordered lists
    - [x] Reference resolution
    - [x] Horizontal rule
    - [x] Paragraph
    - [x] Nested blocks
- [ ] Span
    - [x] Emphasis
    - [x] Strong
    - [x] Code
    - [x] Link
    - [x] Image
    - [x] Nested spans
    - [ ] HTML
- [x] Additional functionality
    - [x] simplify ast
    - [x] backward parsing

### Syntax Guide

#### Block

##### Header

A header is a block-level element which can contain multiple span-level elements.

###### Atx-Style

You can create an Atx-Style-Header by starting a line with multiple `#`-signs followed by a blank. The more hashtag signs you write
the lower is the level of the produced header:

```
# Header level 1
## Header level 2
```
The lowest level of an Atx-Style-Header is `6`. All headers which start with more than six `#`-signs will automatically result in level
six headers.  

###### Setext-Style-Header

You can create a Setext-Style-Header by writing a line with the header text which is immediately followed by a line with minimum 3 starting `-`-signs or `=`-signs.
Setext-Style-Headers only support two levels:

```
Header level 1
===
Header level 2
---
```

Finishing a Setext-Style-Header with `=`-signs will result in a level one header, while finishing a Setext-Style-Header with `-`-signs will result in a level two header.

##### Quotes

You can create a Quote by starting a line with a `>`-sign. All immediately following lines which also start with a `>`-sign will belong to that quote. A Quote can contain code-blocks and paragraphs.

```
> This is a single-line-quote

> This qoute goes
> over multiple lines
```

##### Unordered list

You can create an unordered list by starting a line with either a `-`-sign, `+`-sign, or `*`-sign followed by a blank. The chosen sign for the first list item is called the starter pattern. Every following line which doesn't begin with the starter pattern belongs will be added to the current list item. To start another list item in the same list you simply start a new line with the starter pattern. An unordered list can contain one to multiple list items, which can contain one to multiple block-level-elements. If a list item is followed by two blank lines or one blank line and a line which doesn't start with the starter pattern, the list is considered finished.

```
- Start a list with this pattern
- Create a new list item by repeating the pattern
  As long as you don't start a line with the pattern
  all lines belong to the list item.
- You can nest lists by intending the starter pattern of the inner list
  - This is
  - the inner list


- This is a
- new list

- this is not
```

##### Ordered list

You can create an ordered list by starting a line with a number followed by a dot and a blank. The enumeration of the list items will start at the first items number. All following numbers are ignored. You can use an ordered list the same way as an unordered list.

```
1. Start a list with this pattern
1. this number is ignored and will result in 2.
3. Create a new list item by repeating the pattern
   As long as you don't start a line with the pattern
   all lines belong to the list item.
1. You can nest lists by intending the starter pattern of the inner list
   1. This is
   5. the inner list


1. This is a
1. new list

2. this not
```

##### Reference Resolution

A Reference-Resolution-Block provides references urls which can be used in link- or image-spans by stating the reference-id of the block.
These blocks will not result in any output and are only used to provide document wide url-references.

```
[reference-id]: url/to/resource
```

You can reference a url by providing the reference-id in square brackets.

```
[I'm a link with reference-id][reference-id]
![I'm an image with reference-id][reference-id]
```

##### Horizontal Rule

You can create a horizontal rule by writing a blank line which is immediately followed by a line which starts with at least three `-`-signs, `_`-signs, or `*`-signs.

```

***

---

___
```

##### Paragraph

Lines which can't be assigned to a block will result in a paragraph. A paragraph can be seen as plain (formatted) text, which is why a paragraph block can only contain span-level elements.

#### Span

##### Emphatic stress

You can emphasize specific parts of a text by wrapping the part either into a `*`-sign or a `_`-sign. A emphasized text sequence can contain other span-level-elements.  
**NOTICE**: Emphasized text inside emphasized text will always be interpreted as _text_.

```
This is normal text *this* and _this_ will be emphasized
```

##### Strong importance

You can highlight text with strong importance by wrapping the part either into two `*`-signs or two `_`-signs. A text sequence with strong importance can contain other span-level-elements.  **NOTICE**: A text span with strong importance inside of a text span with strong importance will always be interpreted as _text_.

```
This is normal text **this** and __this__ will be displayed as strong importance
```

##### Link

A link consists of two parts:
- The link title
- The link url _(optional)_

There are three different ways to display a link in markdown:

```
[Provide the url in curved brackets](link/one)
[Provide the url in curved and angle brackets](<link/two>)
[Provide the url with a reference-id][reference-id]
```

The text sequence in the first square brackets is the title, which can contain other span-level-elements.

##### Image

An image consists of three parts:
- The link title _(optional)_
- The link url _(optional)_
- The alternative text

There are three different ways to display an image in markdown:

```
![Alternative text](url/to/image/one "optional title")
![Alternative text](<url/to/image/two> "optional title")
![Alternative text][reference-id]
```

The text sequence in the first square brackets is the alternative text, other than the link it can **just contain text**.

##### Inline Code

You can create inline-code sequences by wrapping a sequence of text into `` ` ``-signs. All the text which is between the backticks will be interpreted as plain text. You can vary the amount of backticks, so you can also display backticks in your code.

```
`simple code sequence`
``simple code sequence which contains a `-sign ``
```

## License

Copyright (c) 2018 SAP SE or an SAP affiliate company. All rights reserved.

This file is licensed under the Apache Software License, v. 2 except as noted otherwise in the [LICENSE](/LICENSE.txt) file
