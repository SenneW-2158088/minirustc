%{
    #include "lexer/Scanner.h"
    #include "driver/Driver.h"

    std::string symbol{};
    std::string suffix{};

    #undef YY_DECL
    #define YY_DECL MRC::Token MRC::Scanner::scan()
%}

%option c++
%option yyclass="MRC::Scanner"
%option nodefault noyywrap

/* Type definitions */
BIN_DIGIT   [0-1]
OCT_DIGIT   [0-7]
DEC_DIGIT   [0-9]
HEX_DIGIT   [0-9 a-f A-F]

/* Literal definitions */
BIN_LIT     0b{BIN_DIGIT}+
OCT_LIT     0o{OCT_DIGIT}+
HEX_LIT     0x{HEX_DIGIT}+
DEC_LIT     {DEC_DIGIT}+
INTEGER     ({BIN_DIGIT}+|{OCT_DIGIT}+|{DEC_DIGIT}+|{HEX_DIGIT}+)

/* Suffixes */
INT_SUFFIX  (i8|i16|i32|i64|i128|isize|u8|u16|u32|u64|u128|usize)
FLOAT_SUFFIX (f32|f64|e[0-9]+)

%x EXPECT_INT_SUFFIX

%%

%{
    #undef YY_USER_ACTION
    #define YY_USER_ACTION  \
        m_driver.location.step(); \
        m_driver.location.columns(yyleng);

%}

{INTEGER}    {
    symbol = yytext;
    BEGIN(EXPECT_INT_SUFFIX);
}

<EXPECT_INT_SUFFIX>{INT_SUFFIX}    {
    BEGIN(INITIAL);
    return MRC::Token(symbol, yytext, MRC::Parser::token_type::TOKEN_INTEGER_LITERAL, m_driver.location);
}

<EXPECT_INT_SUFFIX>{FLOAT_SUFFIX}  {
    BEGIN(INITIAL);
    return MRC::Token(symbol, yytext, MRC::Parser::token_type::TOKEN_INTEGER_LITERAL, m_driver.location);
}
.                       { /* Skip everything else */ }
<<EOF>>                 { return MRC::Token(); }
%%
