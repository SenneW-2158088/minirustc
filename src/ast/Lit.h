#pragma once

#include "Token.h"
#include "util/util.h"
#include <ostream>
#include <string>
#include <variant>

// namespace MRC { class Parser; }

namespace MRC::AST {

struct IntegerLit {};
struct BooleanLit {};
struct FloatLit {};
struct StrLit {};

struct Lit {
  using LiteralKind = std::variant<IntegerLit, BooleanLit, StrLit, FloatLit>;
  Symbol symbol;
  Opt<Symbol> suffix;
  LiteralKind kind;

public:
  Lit() : kind() {}

  explicit Lit(Symbol symbol, Opt<Symbol> suffix, LiteralKind k)
      : symbol(symbol), suffix(suffix), kind(std::move(k)) {}

  static Lit makeInteger(Token token) {
    return Lit(token.symbol, token.suffix, IntegerLit());
  }

  static Lit makeBoolean(Token token) {
    return Lit(token.symbol, token.suffix, BooleanLit());
  }

  static Lit makeFloat(Token token) {
    return Lit(token.symbol, token.suffix, FloatLit());
  }

  static Lit makeStr(Token token) {
    return Lit(token.symbol, token.suffix, StrLit());
  }
};
} // namespace MRC::AST
