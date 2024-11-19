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
HEX_DIGIT   [0-9a-fA-F]

ESC_SEQ     \\([\"\\nrt0]|x[0-9a-fA-F]{2}|u\{[0-9a-fA-F]{1,6}\})
STR_CHAR    [^\"\\\n]|{ESC_SEQ}

/* Literal definitions */
BIN_LIT     0b{BIN_DIGIT}+
OCT_LIT     0o{OCT_DIGIT}+
HEX_LIT     0x{HEX_DIGIT}+
DEC_LIT     {DEC_DIGIT}+
STR_LIT     \"({STR_CHAR})*\"

/* Improved float pattern */
INTEGER     ({DEC_LIT}|{HEX_LIT}|{OCT_LIT}|{BIN_LIT})
FLOAT       {DEC_LIT}(\.{DEC_LIT}?)?
STRING      {STR_LIT}

/* Suffixes */
INT_SUFFIX      (i8|i16|i32|i64|u8|u16|u32|u64|usize)
FLOAT_SUFFIX    (f32|f64)

%%

%{
    #undef YY_USER_ACTION
    #define YY_USER_ACTION  \
        m_driver.location.step(); \
        m_driver.location.columns(yyleng);

%}

{INTEGER} {
    std::cout << "got integer: " << yytext << std::endl;
    symbol = yytext;
    return MRC::Token(symbol, "", MRC::Parser::token_type::TOKEN_INTEGER_LITERAL, m_driver.location);
}

{FLOAT} {
    std::cout << "got float: " << yytext << std::endl;
    symbol = yytext;
    return MRC::Token(symbol, "", MRC::Parser::token_type::TOKEN_FLOAT_LITERAL, m_driver.location);
}

{STRING} {
    std::cout << "got string: " << yytext << std::endl;
    symbol = yytext;
    return MRC::Token(symbol, "", MRC::Parser::token_type::TOKEN_STR_LITERAL, m_driver.location);
}

[ \t\n]+        { /* Ignore whitespace */ }
.               { /* Error case */
    std::cerr << "Unexpected character: " << yytext << std::endl;
    return MRC::Token("", "", MRC::Parser::token_type::TOKEN_ERROR, m_driver.location);
}

<<EOF>>         { return MRC::Token("", "", MRC::Parser::token_type::TOKEN_EOF, m_driver.location); }

%%
