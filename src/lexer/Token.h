#pragma once

#include "util/util.h"
#include "parser/location.hh"
#include <string>

namespace MRC {

    using Symbol = std::string;
    using TokenType = int;

    struct Token {
        Symbol symbol;
        Opt<Symbol> suffix;

        TokenType type;
        location span;

        Token() : symbol(), suffix() {}
        Token(
            Symbol symbol,
            Opt<Symbol> suffix,
            TokenType type,
            location span
        ) : symbol(symbol), suffix(suffix) {}
    };
}

inline std::ostream& operator<<(std::ostream& os, const MRC::Token& token) {
    os << "Token{symbol=\"" << token.symbol << "\"";
    if (token.suffix) {
        os << ", suffix=\"" << *token.suffix << "\"";
    }
    os << ", type=" << token.type
       << ", span=" << token.span;
    os << "}";
    return os;
}
