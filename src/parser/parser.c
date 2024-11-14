%require "3.2"
%language "c++"

%code requires {

    namespace MRC {
        class Scanner; // Forward declaration
        struct Location;
    }

}

%code top {
    #include <iostream>
    #include "lexer/Scanner.hpp"
    #include "parser/parser.h"
}


%header
%verbose

%lex-param { EzAquarii::Scanner &scanner }
%parse-param { MRC::Scanner  &scanner }


%code {
    static MRC::Parser::symbol_type yylex(MRC::Scanner &scanner) {
        return scanner.scan();
    }
}


%define api.namespace { MRC }
%define api.parser.class { Parser }
// %define api.location.type { Location }

%define api.value.type variant
%define api.token.prefix {TOKEN_}
%define api.token.constructor
%define api.token.raw

%locations

%defines

%token TEST
%token EOF

%%
program
    : { std::cout << "Hellof" << std::endl;}
    | TEST
    ;
%%

namespace MRC {
    void Parser::error(const location &location, const std::string &message)
    {
        std::cerr << "Error at lines " << location << ": " << message << std::endl;
    }
}
