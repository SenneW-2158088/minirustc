%require "3.2"
%language "c++"

%code requires {

    #include "ast/Ast.h"
    #include "lexer/Token.h"


    namespace MRC {
        class Scanner; // Forward declaration
        class Driver;
        struct Token;
        struct Location;
    }

    namespace MRC::AST {
        struct Ast;
        struct Expr;
        struct Stmt;
        struct Lit;
    }
}

%code top {
    #include <iostream>
    #include "driver/Driver.h"
    #include "lexer/Scanner.h"
    #include "parser/parser.h"
    #include "lexer/Token.h"
}


%header
%verbose

%lex-param { MRC::Scanner *scanner }
%parse-param { MRC::Driver  *driver }
%parse-param { MRC::Scanner *scanner }
%parse-param { MRC::AST::Ast *ast }


%code {
    #define yylex scanner->yylex
    using Tok = MRC::Token;
}

%define api.namespace { MRC }
%define api.parser.class { Parser }
// %define api.location.type { Location } // Maybe later for custom locations

%define api.value.type variant
%define api.token.prefix {TOKEN_}
%define api.token.raw

%locations

%defines

/* Token Definitions */
// Integers
%token <MRC::Token> INTEGER_LITERAL
%token <MRC::Token> BOOLEAN_LITERAL
%token <MRC::Token> FLOAT_LITERAL
%token <MRC::Token> STR_LITERAL

// Misc
%token ERROR    1
%token EOF      0

%type <std::string>  statement
%type <std::string>  expression
%type <MRC::AST::Lit>   literal

%%
program
    : statement EOF              { // std::cout << "expr" << std::endl;
    }
    | EOF                        { // std::cout << "empty file" << std::endl;
    }
    ;

/* Statements */
statement
    : expression                {
        // std::cout << "Parsing expression" << std::endl;
        $$ = $1;
    }
    ;

/* Expressions */
expression
    : literal                   {
        // std::cout << "Parsing literal" << std::endl;
        $$ = "Expr";
        // std::cout << $1 << std::endl;
    }
    ;

literal
    : INTEGER_LITERAL           { $$ = MRC::AST::Lit::makeInteger($1); std::cout << $1 << std::endl;}
    | FLOAT_LITERAL             { $$ = MRC::AST::Lit::makeFloat($1); std::cout << $1 << std::endl;}
    | STR_LITERAL            { $$ = MRC::AST::Lit::makeStr($1); std::cout << $1 << std::endl;}
    ;

%%

namespace MRC {
    void Parser::error(const location &location, const std::string &message)
    {
        std::cerr << "Error at lines " << location << ": " << message << std::endl;
    }
}
