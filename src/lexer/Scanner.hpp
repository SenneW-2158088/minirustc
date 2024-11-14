#pragma once

#if !defined(yyFlexLexerOnce)
#include <FlexLexer.h>
#endif

#include "parser/parser.h"

namespace MRC {
class Scanner : public yyFlexLexer {
public:
    Scanner() {}

    virtual int yylex(Parser::semantic_type *const lval, Parser::location_type *location);

    virtual ~Scanner() {}

private:
  Parser::semantic_type *yylval = nullptr;
};
} // namespace MRC
