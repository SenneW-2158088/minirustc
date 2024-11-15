%{
    #include "lexer/Scanner.hpp"
    #include "driver/Driver.hpp"
%}

%option c++
%option yyclass="MRC::Scanner"
%option nodefault noyywrap

%%

%{
    using Token = MRC::Parser::token; // TODO: REMOVE
    #undef YY_USER_ACTION
    #define YY_USER_ACTION m_driver.location.step();  m_driver.location.columns(yyleng); std::cout << yytext << " " << m_driver.location << std::endl;

%}

"test"      { return MRC::Parser::make_TEST(m_driver.location); }
.           { /* Skip everything else */ }
<<EOF>>     { return MRC::Parser::make_EOF(m_driver.location); }
%%
