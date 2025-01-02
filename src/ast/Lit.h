#pragma once

#include "Token.h"
#include "typechecking/Type.h"
#include "util/util.h"
#include <exception>
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
  TS::CheckType check_type;

public:
  explicit Lit(Symbol symbol, Opt<Symbol> suffix, LiteralKind k, TS::CheckType type)
      : symbol(std::move(symbol)), suffix(std::move(suffix)), kind(std::move(k)), check_type(std::move(type)) {}
    static Lit makeInteger(Token token) {
        std::regex r("([0-9]+(?:_[0-9]+)*)((?:i|u)(?:8|16|32|64|size))?");
        std::smatch matches;
        if (std::regex_search(token.symbol, matches, r)) {
            if(matches[2].str().empty()) {
                return Lit(matches[1].str(), "", IntegerLit(), TS::CheckType::makeVar(TS::Type::makeInt(true, 32))); // default to i32
            }
            else {
                std::string suffix = matches[2].str();
                bool sign = suffix[0] == 'i';
                std::string bits_str = suffix.substr(1);
                int bits = bits_str == "size" ? 64 : std::stoi(bits_str); // handle usize case
                return Lit(matches[1].str(), suffix, IntegerLit(), TS::CheckType::makeVar(TS::Type::makeInt(sign, bits)));
            }
        }
        throw std::exception();
    }

  static Lit makeBoolean(Token token) {
    return Lit(token.symbol, token.suffix, BooleanLit(), TS::CheckType::makeConcrete(TS::Type::makeBool()));
  }

  static Lit makeFloat(Token token) {
    return Lit(token.symbol, token.suffix, FloatLit(), TS::CheckType::makeVar(TS::Type::makeFloat(32)));
  }

  static Lit makeStr(Token token) {
    return Lit(token.symbol, token.suffix, StrLit(), TS::CheckType::makeVar(TS::Type::makeString()));
  }
};
} // namespace MRC::AST
