%{
    #include "lexer/Scanner.hpp"
    using namespace MRC;

    #undef  YY_DECL
    #define YY_DECL int Scanner::yylex(Parser::semantic_type *yylval, Parser::location_type *yylloc)
%}

%option c++
%option yyclass="MRC::Scanner"
%option nodefault noyywrap

%%

%{
    using Token = MRC::Parser::token;
%}

"test"    { return Token::TEST; }
.         { /* Skip everything else */ }
%%
