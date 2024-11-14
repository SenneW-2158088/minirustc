%require "3.2"
%language "c++"

%code requires {

    namespace MRC {
        class Scanner; // Forward declaration
    }

}

%code top {
    #include <iostream>
    #include "lexer/Scanner.hpp"
    #include "parser/parser.h"
    #include "location.hh"
}


%header
%verbose

%parse-param { MRC::Scanner  &scanner  }

%code {
    #define yylex scanner.yylex
}


%define api.parser.class { Parser }
%define api.namespace { MRC }
%locations


%defines

%token TEST

%%
program
    : /* empty */
    | program TEST
    ;
%%

namespace MRC {
    void Parser::error(const location &location, const std::string &message)
    {
        std::cerr << "Error at lines " << location << ": " << message << std::endl;
    }
}
