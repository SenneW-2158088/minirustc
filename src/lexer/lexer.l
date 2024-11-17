%{
    #include "lexer/Scanner.hpp"
    #include "driver/Driver.hpp"
%}

%option c++
%option yyclass="MRC::Scanner"
%option nodefault noyywrap

/* Type definitions */
BIN_DIGIT   [0-1]
OCT_DIGIT   [0-7]
DEC_DIGIT   [0-9]
HEX_DIGIT   [0-9 a-f A-F]
INTEGER     ({BIN_DIGIT}+|{OCT_DIGIT}+|{DEC_DIGIT}+|{HEX_DIGIT}+)

%%

%{
    #undef YY_USER_ACTION
    #define YY_USER_ACTION m_driver.location.step();  m_driver.location.columns(yyleng);
%}

{INTEGER}+    { int lex = std::stoi(yytext); return MRC::Parser::make_DEC_DIGIT(lex, m_driver.location); }
"test"      { return MRC::Parser::make_TEST(m_driver.location); }
.           { /* Skip everything else */ }
<<EOF>>     { return MRC::Parser::make_EOF(m_driver.location); }
%%
