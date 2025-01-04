#pragma once

#include "Token.h"
#include "typechecking/Type.h"
#include "util/util.h"
#include <exception>
#include <regex>
#include <string>
#include <variant>
#include "ast/prelude.h"

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
  Id id;

public:
  explicit Lit(Id id, Symbol symbol, Opt<Symbol> suffix, LiteralKind k, TS::CheckType type)
      : id(id), symbol(std::move(symbol)), suffix(std::move(suffix)), kind(std::move(k)), check_type(std::move(type)) {}

    static Lit makeInteger(Id id, Token token) {
        // Match digits, optional underscore, and optional suffix
        std::regex r("([0-9]+)(_)?([ui](?:8|16|32|64))?");
        std::smatch matches;
        if (std::regex_search(token.symbol, matches, r)) {
            if(matches[3].str().empty()) {
                return Lit(id, matches[1].str(), "", IntegerLit(),
                          TS::CheckType::makeVar(TS::Type::makeInt(true, 32)));
            } else {
                std::string suffix = matches[3].str();
                bool sign = suffix[0] == 'i';
                std::string bits_str = suffix.substr(1);
                int bits;
                bits = std::stoi(bits_str);
                return Lit(id, matches[1].str(), suffix, IntegerLit(),
                          TS::CheckType::makeConcrete(TS::Type::makeInt(sign, bits)));
            }
        }
        throw std::exception();
    }

  static Lit makeBoolean(Id id, Token token) {
    return Lit(id, token.symbol, token.suffix, BooleanLit(), TS::CheckType::makeConcrete(TS::Type::makeBool()));
  }

  static Lit makeFloat(Id id, Token token) {
      std::regex r("([0-9]+(?:_[0-9]+)*(?:\\.[0-9]+(?:_[0-9]+)*)?)(f(?:32|64))?");
      std::smatch matches;
      if (std::regex_search(token.symbol, matches, r)) {
          if(matches[2].str().empty()) {
              return Lit(id, matches[1].str(), "", FloatLit(), TS::CheckType::makeVar(TS::Type::makeFloat(64))); // default to f64
          }
          else {
              std::string suffix = matches[2].str();
              int bits = std::stoi(suffix.substr(1));
              return Lit(id, matches[1].str(), suffix, FloatLit(), TS::CheckType::makeVar(TS::Type::makeFloat(bits)));
          }
      }
      throw std::exception();
  }

  static Lit makeStr(Id id ,Token token) {
    return Lit(id, token.symbol, token.suffix, StrLit(), TS::CheckType::makeVar(TS::Type::makeString()));
  }
};
} // namespace MRC::AST
