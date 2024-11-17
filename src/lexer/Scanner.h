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

    static int yylex(MRC::Token* val, MRC::Parser::location_type* loc, Scanner *scanner) {
            *val = scanner->scan();  // Get the token
            // if (loc) *loc = val->location;  // Update location if needed
            // return val->type;  // Return the token type
            return 0;
    }
    virtual MRC::Token scan();
    virtual ~Scanner() {}

   private:
   Driver &m_driver;
   Parser::semantic_type* yylval = nullptr;
};
}  // namespace MRC
