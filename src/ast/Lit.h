#pragma once

#include "Token.h"
#include "util/util.h"
#include <exception>
#include <ostream>
#include <regex>
#include <string>
#include <variant>

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
    std::regex r("([0-9]+)(_*)([ui][0-9]{2})*"); // Made the suffix group non-capturing
    std::smatch matches;
    if (std::regex_search(token.symbol, matches, r)) {
      // std::cout << token.symbol << std::endl;
      std::cout << matches[1].str() << std::endl;
      std::cout << matches[3].str() << std::endl;
      std::cout << "ahhhhhh" << std::endl;
      return Lit(matches[1].str(), matches[3].str(), IntegerLit());
    }
    std::cout << "invalid" << std::endl;
    throw std::exception();
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
