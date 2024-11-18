#pragma once

#if !defined(yyFlexLexerOnce)
#include <FlexLexer.h>
#endif

#include "Token.h"
#include "parser/parser.h"

namespace MRC {
class Scanner : public yyFlexLexer {
   public:
    Scanner(Driver &driver) : m_driver(driver) {}

    int yylex(Parser::value_type* val, MRC::Parser::location_type* loc, Scanner *scanner) {
        MRC::Token token = scanner->scan();
        val->build(token);
        return token.type;  // Return the token type
    }
    virtual MRC::Token scan();
    virtual ~Scanner() {}

   private:
   Driver &m_driver;
};
}  // namespace MRC
