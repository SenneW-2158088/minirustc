# Basic

- [x] i32 + bools
- [x] Integer literals
- [x] Bool literals
- [x] let, let mut statements

# DONE

* Create and test parsing of all basic test cases
* Complete the russt specific type declarations
* Start type checker, data structures + basic Typed Syntax Tree (TST)

# TODO

* Parse specific rust types
* Start a simple interpreter

## Thinkpad

I need to statically typecheck and inference types of
expressions and parameters. While also making an easy to use
representation of statements and expressions,... to execute in the interpreters.

1) Iteratively walk through the tree and insert a interpreter friendly type
for each statement,expression,..
2) also gather/infer the type. If the type isn't directly inferrable,
use a callback system to assign type when inferred.
3) Keep track of a context including a symbol table to check for literal conflicts.
try to embed the type checking and symbols.
