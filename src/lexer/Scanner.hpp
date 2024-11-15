#pragma once

#if !defined(yyFlexLexerOnce)
#include <FlexLexer.h>
#endif

#undef YY_DECL
#define YY_DECL MRC::Parser::symbol_type MRC::Scanner::scan()

#include "parser/parser.h"

namespace MRC {
class Scanner : public yyFlexLexer {
   public:
    Scanner(Driver &driver) : m_driver(driver) {}

    virtual Parser::symbol_type scan();
    virtual ~Scanner() {}

   private:
   Driver &m_driver;
   Parser::semantic_type* yylval = nullptr;
};
}  // namespace MRC
