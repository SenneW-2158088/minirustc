%{
    #include "lexer/Scanner.hpp"
%}

%option c++
%option yyclass="MRC::Scanner"
%option nodefault noyywrap

%%

%{
    using Token = MRC::Parser::token; // TODO: REMOVE
%}

"test"      { return MRC::Parser::make_TEST(MRC::location()); }
<<EOF>>     { return MRC::Parser::make_EOF(MRC::location()); }
.           { /* Skip everything else */ }
%%
