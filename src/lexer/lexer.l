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
BIN_DIGIT       [0-1]
OCT_DIGIT       [0-7]
DEC_DIGIT       [0-9]
HEX_DIGIT       [0-9a-fA-F]
LETTER          [a-zA-Z_]

ESC_SEQ         \\([\"\\nrt0]|x[0-9a-fA-F]{2}|u\{[0-9a-fA-F]{1,6}\})
STR_CHAR        [^\"\\\n]|{ESC_SEQ}

LINE_COMMENT    \/\/[^\n]*
BLOCK_COMMENT   \/\*([^*]|\*+[^*/])*\*+\/

/* Literal definitions */
BIN_LIT         0b{BIN_DIGIT}+
OCT_LIT         0o{OCT_DIGIT}+
HEX_LIT         0x{HEX_DIGIT}+
DEC_LIT         {DEC_DIGIT}+
STR_LIT         \"([^\\\"]|\\.)*\"

/* Improved float pattern */
INTEGER         {DEC_LIT}|{HEX_LIT}|{OCT_LIT}|{BIN_LIT}
FLOAT           {DEC_LIT}\.{DEC_LIT}?
STRING          {STR_LIT}
IDENTIFIER      {LETTER}({LETTER}|{DEC_DIGIT})*

/* Suffixes */
INT_SUFFIX      (i8|i16|i32|i64|u8|u16|u32|u64|usize)
FLOAT_SUFFIX    (f32|f64)

%%


%{
    #undef YY_USER_ACTION
    #define YY_USER_ACTION  \
        m_driver.location.step(); \
        m_driver.location.columns(yyleng);

    using Token = MRC::Parser::token_type;

%}


"+"         { return MRC::Token(yytext, "", Token::TOKEN_PLUS, m_driver.location); }
"-"         { return MRC::Token(yytext, "", Token::TOKEN_MINUS, m_driver.location); }
"*"         { return MRC::Token(yytext, "", Token::TOKEN_STAR, m_driver.location); }
"/"         { return MRC::Token(yytext, "", Token::TOKEN_SLASH, m_driver.location); }
"%"         { return MRC::Token(yytext, "", Token::TOKEN_PERCENT, m_driver.location); }
"="         { return MRC::Token(yytext, "", Token::TOKEN_EQ, m_driver.location); }

"=="         { return MRC::Token(yytext, "", Token::TOKEN_EQEQ, m_driver.location); }
"+="        { return MRC::Token(yytext, "", Token::TOKEN_PLUSEQ, m_driver.location); }
"-="        { return MRC::Token(yytext, "", Token::TOKEN_MINUSEQ, m_driver.location); }
"*="        { return MRC::Token(yytext, "", Token::TOKEN_STAREQ, m_driver.location); }
"/="        { return MRC::Token(yytext, "", Token::TOKEN_SLASHEQ, m_driver.location); }
"%="        { return MRC::Token(yytext, "", Token::TOKEN_PERCENTEQ, m_driver.location); }

"!"         { return MRC::Token(yytext, "", Token::TOKEN_NOT, m_driver.location); }

"&&"        { return MRC::Token(yytext, "", Token::TOKEN_ANDAND, m_driver.location); }
"||"        { return MRC::Token(yytext, "", Token::TOKEN_OROR, m_driver.location); }
"!="        { return MRC::Token(yytext, "", Token::TOKEN_NOTEQ, m_driver.location); }
"&="        { return MRC::Token(yytext, "", Token::TOKEN_ANDEQ, m_driver.location); }
"|="        { return MRC::Token(yytext, "", Token::TOKEN_OREQ, m_driver.location); }
">"         { return MRC::Token(yytext, "", Token::TOKEN_GT, m_driver.location); }
"<"         { return MRC::Token(yytext, "", Token::TOKEN_LT, m_driver.location); }
">="        { return MRC::Token(yytext, "", Token::TOKEN_GTEQ, m_driver.location); }
"<="        { return MRC::Token(yytext, "", Token::TOKEN_LTEQ, m_driver.location); }

","         { return MRC::Token(yytext, "", Token::TOKEN_COMMA, m_driver.location); }
";"         { return MRC::Token(yytext, "", Token::TOKEN_SEMI, m_driver.location); }
":"         { return MRC::Token(yytext, "", Token::TOKEN_COLON, m_driver.location); }
"->"        { return MRC::Token(yytext, "", Token::TOKEN_RIGHT_ARROW, m_driver.location); }
"<-"        { return MRC::Token(yytext, "", Token::TOKEN_LEFT_ARROW, m_driver.location); }

"("        { return MRC::Token(yytext, "", Token::TOKEN_LPAREN, m_driver.location); }
")"        { return MRC::Token(yytext, "", Token::TOKEN_RPAREN, m_driver.location); }
"{"        { return MRC::Token(yytext, "", Token::TOKEN_LBRACKET, m_driver.location); }
"}"        { return MRC::Token(yytext, "", Token::TOKEN_RBRACKET, m_driver.location); }
"["        { return MRC::Token(yytext, "", Token::TOKEN_LBRACE, m_driver.location); }
"]"        { return MRC::Token(yytext, "", Token::TOKEN_RBRACE, m_driver.location); }

"break"     { return MRC::Token(yytext, "", Token::TOKEN_BREAK, m_driver.location); }
"continue"  { return MRC::Token(yytext, "", Token::TOKEN_CONTINUE, m_driver.location); }
"return"    { return MRC::Token(yytext, "", Token::TOKEN_RETURN, m_driver.location); }
"if"        { return MRC::Token(yytext, "", Token::TOKEN_IF, m_driver.location); }
"else"      { return MRC::Token(yytext, "", Token::TOKEN_ELSE, m_driver.location); }
"for"       { return MRC::Token(yytext, "", Token::TOKEN_FOR, m_driver.location); }
"while"     { return MRC::Token(yytext, "", Token::TOKEN_WHILE, m_driver.location); }
"loop"      { return MRC::Token(yytext, "", Token::TOKEN_LOOP, m_driver.location); }
"fn"        { return MRC::Token(yytext, "", Token::TOKEN_FN, m_driver.location); }
"let"       { return MRC::Token(yytext, "", Token::TOKEN_LET, m_driver.location); }
"mut"       { return MRC::Token(yytext, "", Token::TOKEN_MUT, m_driver.location); }
"true"      { return MRC::Token(yytext, "", Token::TOKEN_BOOLEAN_LITERAL, m_driver.location); }
"false"     { return MRC::Token(yytext, "", Token::TOKEN_BOOLEAN_LITERAL, m_driver.location); }
"println!"     { return MRC::Token(yytext, "", Token::TOKEN_PRINT, m_driver.location); }

"::"     { return MRC::Token(yytext, "", Token::TOKEN_SCOPE_RESOLUTION, m_driver.location); }

{LINE_COMMENT}  { /* Ignore line comments */ }
{BLOCK_COMMENT} { /* Ignore block comments */ }

{STR_LIT} {
    symbol = yytext;
    return MRC::Token(yytext, "", Token::TOKEN_STR_LITERAL, m_driver.location);
}

{IDENTIFIER} {
    return MRC::Token(yytext, "", Token::TOKEN_IDENTIFIER, m_driver.location);
}

{DEC_LIT}(_)*({INT_SUFFIX})? {
    return MRC::Token(yytext, "", Token::TOKEN_INTEGER_LITERAL, m_driver.location);
}

{DEC_LIT}{FLOAT_SUFFIX} {
    return MRC::Token(yytext, "", Token::TOKEN_FLOAT_LITERAL, m_driver.location);
}

{DEC_LIT}\.{DEC_LIT}{FLOAT_SUFFIX}? {
    return MRC::Token(yytext, "", Token::TOKEN_FLOAT_LITERAL, m_driver.location);
}


[ \t\n]+        { /* Ignore whitespace */ }
.               { /* Error case */
    std::cerr << "Unexpected character: " << yytext << std::endl;
    return MRC::Token("", "", Token::TOKEN_ERROR, m_driver.location);
}

<<EOF>>         { return MRC::Token("", "", Token::TOKEN_EOF, m_driver.location); }

%%
