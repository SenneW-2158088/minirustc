# Mini rust compiler

## Installation & building

The project uses cmake to generate build files. With the following commands, the project can be built.

```sh
cmake -B build
cd build && make
```

If you use nix, you can build the package with

```sh
nix build -o minirust
```

## Project structure

The project is divided into seperate modules:

- **lexer**: Contains the lexfile and tokens.
- **parser**: Contains the bison grammar file.
- **ast**: Contains the AST data structures.
- **driver**: Contains the main driver to execute and analyse the program

## Resources

- [Flex & Bison in C++](https://www.jonathanbeard.io/tutorials/FlexBisonC++)
