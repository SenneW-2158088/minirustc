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
    #include "driver/Driver.h"
    #include "lexer/Scanner.h"
    #include "parser/parser.h"

    // Let's say that every token is of type string and later add a custom token type if needed
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
%token  <std::string>   INTEGER_LITERAL

// Misc
%token TEST 1
%token EOF  0

%type <std::string> statement
%type <std::string> expression
%type <std::string> literal_expression


%%
program
    : statement EOF              { std::cout << "expr" << std::endl; driver->update_location(); }
    | EOF                        { std::cout << "empty file" << std::endl; }
    ;

/* Statements */
statement
    : expression
    ;

/* Expressions */
expression
    : literal_expression
    ;

literal_expression
    : INTEGER_LITERAL
    ;

%%

namespace MRC {
    void Parser::error(const location &location, const std::string &message)
    {
        std::cerr << "Error at lines " << location << ": " << message << std::endl;
    }
}
