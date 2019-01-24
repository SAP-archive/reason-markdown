# vfmd_parser

## Table of Contents

1. [Description](#description)
2. [Requirements](#requirements)
3. [Download and Installation](#download-and-installation)
4. [Known Issues](#known-issues)
5. [How to obtain support](#how-to-obtain-support)
6. [Contributing](#contributing)
7. [To-Do](#to-do)
8. [Version](#version)
9. [Supported syntax elements](#supported-syntax-elements)
    1. [Block](#block)  
    2. [Span](#span)
10. [License](#license)

## Description

This repository contains a parser for markdown documents provided in vfmd syntax. The parser is written in [Reason](reasonml.github.io) and aligned on [the vfmd specification](http://www.vfmd.org/vfmd-spec/specification/).  
This library is compiled to `JavaScript` by the `BuckleScript`-Compiler and uses `JavaScript-Regexes`.

## Requirements

This project doesn't depend on any third-party library or SAP product.

## Download and Installation

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

Please ask questions or report bugs via `GitHub issues`

## Contributing

Feel free to open a PullRequest with any suggestion and add a description of the bugs you solved/the features you added.

## To-Do

- [ ] Add missing `Markdown` syntax elements (To see a list of the available elements click [here](#supported-syntax-elements))
- [ ] Make the parser modular, so one can include/exclude syntax elements
- [ ] Make the parser extendable

## Version

The current version of the vfmd parser is 0.1.0.

## Supported syntax elements

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

## Syntax Guide

### Block

#### Header

A Header is a block-level element which can contain multiple span-level elements.

##### Atx-Style

You can create an Atx-Style-Header by starting a line with multiple `#`-signs followed by a blank. The more Hashtags you write
the lower is the level of the produced header:

```
# Header level 1
## Header level 2
```
The lowest level of an Atx-Style-Header is `6`. All Headers which start with more than six `#` will automatically result in level
6 Headers.  

##### Setext-Style-Header

You can create a Stext-Style-Header by writing a line with the Header text which is immediately followed by a line with minimum 3 starting `-` or `=`.
Setext-Style-Headers only support two different levels:

```
Header level 1
===
Header level 2
---
```

Finishing a Setext-Style-Header with `=`-signs will result in a level 1 header, while finishing a Setext-Style-Header with `-`-signs
will result in a level 2 header.

#### Quotes

You can create a Quote by starting a line with a `>`-sign. All immediately following lines which also start with a `>`-sign will belong
to that quote. A Quote can contain code-blocks and paragraphs.

```
> This is a single-line-quote

> This qoute goes
> over multiple lines
```

#### Unordered list

You can create a Unordered list by starting a line with either an `-`-sign, `+`-sign, or `*`-sign followed by a blank. Every following line which doesn't begin with this pattern belongs to one list item. To start another list item in the same list you can just repeat the pattern from the first list item of the list. A Unordered list contains multiple list items, which can contain multiple block-level-elements. You can finish a Unordered list
by writing two following blank lines or one following blank line which is not followed by a line which starts with the starter pattern.

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

#### Ordered list

You can create a Ordered list by starting a line with a number followed by a dot and a blank. The enumeration of the list items will start at the first items number. All following number are ignored. You can use a Ordered list the same way as a Unordered list

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

#### Reference Resolution

A Reference-Resolution-Block provides references to urls which can be used in link- or image-spans by stating the reference-id of the Block.
These blocks will not result in any ouput and are just used to provide urls document wide. You can create a Reference-Resolution-Block like so:

```
[reference-id]: url/to/resource
```

Then you can reference the url in your images and links like so:

```
[I'm a link with reference-id][reference-id]
![I'm an image with reference-id][reference-id]
```

#### Horizontal Rule

You can create a Horizontal rule by writing a blank line which is immediately followed by a line which starts with at least 3 `-`-signs, `_`-signs, or `*`-signs.

```

***

---

___
```

#### Paragraph

A Paragraph is every line which doesn't belong to one of the above block-level elements. A Paragraph can be seen as plain (formatted) text, which
is why a Paragraph block can only contain span-level elements.

### Span

#### Emphatic stress

You can emphase specific parts of a text by wrapping the part either into a `*`-sign or a `_`-sign. A emphased text sequence can contain other span-level-elements.  
**NOTICE**: Emphased text inside emphased text will always be interpreted as _text_.

```
This is normal text *this* and _this_ will be emphased
```

#### Strong importance

You can display specific parts of a text as strong important by wrapping the part either into two `*`-signs or two `_`-signs. A important text sequence can contain other span-level-elements.  **NOTICE**: Important text inside important text will always be interpreted as _text_.

```
This is normal text **this** and __this__ will be displayed as strong importance
```

#### Link

A link consists of two parts:
- The link title
- The link url _(optional)_

There are three different ways to write a link in markdown:

```
[Provide the url in curved brackets](link/one)
[Provide the url in curved and angle brackets](<link/two>)
[Provide the url with a reference-id][reference-id]
```

The text sequence in the first square brackets is the title, which can contain other span-level-elements.

#### Image

An image consists of three parts:
- The link title _(optional)_
- The link url _(optional)_
- The alternative text

There are three different ways to write an image in markdown:

```
![Alternative text](url/to/image/one "optional title")
![Alternative text](<url/to/image/two> "optional title")
![Alternative text][reference-id]
```

The text sequence in the first square brackets is the alternative text, other than the link it can **just contain text**.

#### Inline Code

You can create inline-code sequences by wrapping a sequence of text into `` ` ``-signs. All the text which is between the backticks will be interpreted as plain text. You can vary the amount of backticks, so you can also display backticks in your code.

```
`simple code sequence`
``simple code sequence which contains a `-sign ``
```

## License

Copyright (c) 2018 SAP SE or an SAP affiliate company. All rights reserved.

This file is licensed under the Apache Software License, v. 2 except as noted otherwise in the [LICENSE](/LICENSE.txt) file
