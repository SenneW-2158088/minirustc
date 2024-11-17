%require "3.2"
%language "c++"

%code requires {

    namespace MRC {
        class Scanner; // Forward declaration
        class Driver;
        struct Location;
    }

}

%code top {
    #include <iostream>
    #include "driver/Driver.hpp"
    #include "lexer/Scanner.hpp"
    #include "parser/parser.h"
}


%header
%verbose

%lex-param { MRC::Scanner *scanner }
%parse-param { MRC::Driver  *driver }
%parse-param { MRC::Scanner *scanner }


%code {
    static MRC::Parser::symbol_type yylex(MRC::Scanner *scanner) {
        return scanner->scan();
    }
}


%define api.namespace { MRC }
%define api.parser.class { Parser }
// %define api.location.type { Location } // Maybe later for custom locations

%define api.value.type variant
%define api.token.prefix {TOKEN_}
%define api.token.constructor
%define api.token.raw

%locations

%defines

/* Token Definitions */
// Integers
%token  <int>   DEC_DIGIT
%token  <int>   BIN_DIGIT
%token  <double>   HEX_DIGIT

// Misc
%token TEST 1
%token EOF  0

%type <std::string> expr

%type <std::string> integer_literal
// %type <std::string> decimal_literal
// %type <std::string> binary_literal

%%
program
    : expr EOF              { std::cout << "expr" << std::endl;}
    | EOF                   { std::cout << "empty file" << std::endl; }
    ;

expr
    : TEST expr             { $$ = "friend"; std::cout << "test expr" << @1 << std::endl; }
    | TEST                  { $$ = "friend"; std::cout << "test" << std::endl; }
    | integer_literal expr  { $$ = "friend"; std::cout << "integer literal" << std::endl; }
    ;

/* Literals */
integer_literal
    : DEC_DIGIT     { $$ = "integer literal"; std::cout << "Parsed decimal: " << $1 << std::endl; }
    | HEX_DIGIT     { $$ = "integer literal"; std::cout << "Parsed hexidecimal: " << $1 << std::endl; }
    ;
;

%%

namespace MRC {
    void Parser::error(const location &location, const std::string &message)
    {
        std::cerr << "Error at lines " << location << ": " << message << std::endl;
    }
}
