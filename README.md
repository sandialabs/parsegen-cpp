ParseGen
========

ParseGen is a C++17 library for working with ASCII text formats.
For example, it can parse subsets of formats like XML and YAML.
ParseGen's utility is that it is a toolbox for defining new
text languages and parsing them.

Theory
------

ParseGen is based on classical language theory including
regular languages, regular expressions, finite automata,
context-free grammars, and LALR(1) parsing.
Tokens are described by regular expressions and
languages are expressed as context-free grammars.
ParseGen use finite automaton theory to build table-based
lexers, and uses a fast algorithm from scientific
literature to build a table-based shift-reduce parser
for a context-free grammar.

While we acknowledge that many text formats today are not
based cleanly on context-free language theory, we
believe there is a niche to be filled by a library
like ParseGen that is still based on classical theory.

Implementation
--------------

The key contribution of ParseGen to the software community
is to implement these parser generators are pure C++
objects and functions, as opposed to tools like Flex and Bison
which have custom input formats describing languages and
output generated source code that must then be re-compiled
to obtain a parser.
A language can be built at runtime as a C++ object in ParseGen
and then a parser object can be constructed for that language
and executed repeatedly to parse C++ streams.
This offers maximum flexibility in the workflows users
can employ.

Frequently Asked Questions
--------------------------
1. How do I build it?

ParseGen uses the CMake build system and
tries to be a "standard modern CMake package" as much as possible.

2. Where do I start with the API?

The two most important classes
in ParseGen are the `language`, which fully describes a text language,
and the `parser`, which parses a language according to user-defined
rules that react to syntactic constructs observed.
The file `src/parsegen_calc.cpp` is a great introductory example
that builds a command-line calculator app using ParseGen.

3. Why C++?

C++ is the language of choice in the HPC community
that originated this code.

4. Why C++17?

We use the `std::any` feature of C++17 to allow
users to return any object as the result of parsing some text.

5. Why ASCII only, why not Unicode?

So far avoiding Unicode support has allowed a simple design
and none of the formats we target really need Unicode.
However, we welcome any contributions that move us towards
Unicode support.

At Sandia, ParseGen is SCR# 2564.0
