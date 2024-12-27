#pragma once

#include "Token.h"

namespace MRC::AST {
struct Ident {
  Symbol symbol;

public:
  Ident() = default;
  Ident(Symbol symbol) : symbol(std::move(symbol)) {}
};
} // namespace MRC::AST
