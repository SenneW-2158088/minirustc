%require "3.2"
%language "c++"

%code requires {

    #include "ast/Ast.h"

    namespace MRC {
        class Scanner;
        class Driver;
        struct Token;
        struct Location;
    }
}

%code top {
    #include <iostream>
    #include <memory>

    #include "driver/Driver.h"
    #include "parser/parser.h"
    #include "lexer/Token.h"
    #include "lexer/Scanner.h"

    #include "ast/prelude.h"

    using namespace MRC::AST;
}


%header
%verbose

%lex-param { MRC::Scanner *scanner }
%parse-param { MRC::Driver  *driver }
%parse-param { MRC::Scanner *scanner }
%parse-param { MRC::AST::Ast *ast }


%code {
    #define yylex scanner->yylex
}

%define api.namespace { MRC }
%define api.parser.class { Parser }

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

// Keywoards
%token <MRC::Token> BREAK CONTINUE RETURN
%token <MRC::Token> IF ELSE
%token <MRC::Token> FOR WHILE LOOP
%token <MRC::Token> FN LET MUT
%token <MRC::Token> TRUE FALSE

// Punctuation
%token <MRC::Token> PLUS MINUS STAR SLASH PERCENT CARET
%token <MRC::Token> EQ NE
%token <MRC::Token> PLUSEQ MINUSEQ STAREQ SLASHEQ PERCENTEQ CARETEQ
%token <MRC::Token> NOT AND OR ANDAND OROR SHL SHR
%token <MRC::Token> NOTEQ ANDEQ OREQ
%token <MRC::Token> GT LT GTEQ LTEQ
%token <MRC::Token> COMMA SEMI COLON RIGHT_ARROW LEFT_ARROW
%token <MRC::Token> LPAREN RPAREN LBRACKET RBRACKET LBRACE RBRACE

%token <MRC::Token> IDENTIFIER
%token <MRC::Token> TYPE
%token <MRC::Token> SCOPE_RESOLUTION

// Misc
%token ERROR    1
%token EOF      0

/* Defining Types */

// Statements
%type <MRC::AST::Stmt>              statement
%type <MRC::AST::Block::Statements> statements
%type <MRC::AST::LetStmt>           let_statement

// Expressions
%type <MRC::AST::Expr>              expr
%type <MRC::AST::Expr>              expr_without_block
%type <MRC::AST::Expr>              expr_with_block

%type <U<MRC::AST::Lit>>            literal_expr
%type <MRC::AST::Block>             block_expr

// Identifier
%type <MRC::AST::Ident>             identifier

// Patterns
%type <MRC::AST::Pat>               pattern
%type <MRC::AST::Pat>               identifier_pattern

// Types
%type <MRC::AST::Type>              type
%type <MRC::AST::Path>              type_annotation
%type <std::vector<MRC::Symbol>>    type_path
%type <MRC::Symbol>                 path_segment

// Items
%type <MRC::AST::Item>              item
%type <MRC::AST::Fn>                function
%type <MRC::AST::Fn::Params>        function_parameters
%type <MRC::AST::Param>             function_parameter
%type <MRC::AST::Fn::Body>          function_block

%%

program
    : statement program     { ast->insert(std::move($1));
    }
    | EOF                   { }
    ;

item
    : FN identifier function {
        auto u_fn = std::make_unique<Fn>(std::move($3));
        $$ = Item::makeFn(std::move($2), std::move(u_fn));
    }
    ;

function
    : LPAREN RPAREN function_block {
        $$ = Fn({}, std::move($3));
    }
    | LPAREN RPAREN RIGHT_ARROW type function_block {
        auto uo_type = std::optional(std::make_unique<Type>(std::move($4)));
        $$ = Fn({}, std::move($5), std::move(uo_type));
    }
    | LPAREN function_parameters RPAREN function_block {
        $$ = Fn(std::move($2), std::move($4));
    }
    | LPAREN function_parameters RPAREN RIGHT_ARROW type function_block {
        auto uo_type = std::optional(std::make_unique<Type>(std::move($5)));
        $$ = Fn(std::move($2), std::move($6), std::move(uo_type));
    }
    ;

function_parameters
    : function_parameter {
        $$.push_back(std::move($1));
    }
    | function_parameters COMMA function_parameter {
        $$ = std::move($1);
        $$.push_back(std::move($3));
    }
    ;

function_parameter
    : pattern COLON type {
        auto u_pat = std::make_unique<Pat>(std::move($1));
        auto u_type = std::make_unique<Type>(std::move($3));
        $$ = Param(std::move(u_type), std::move(u_pat));
    }
    ;

function_block
    : block_expr {
        auto u_block = std::make_unique<Block>(std::move($1));
        $$ = std::optional(std::move(u_block));
    }
    | SEMI { $$ = std::nullopt; }
    ;

/* Statements */
statement
    : item
    | let_statement {
        $$ = Stmt(std::move($1));
    }
    ;

let_statement
    : LET pattern SEMI {
        Local local = Local::makeDecl();
        local.pat = std::make_unique<Pat>(std::move($2));
        local.type = std::nullopt;
        auto u_local = std::make_unique<Local>(std::move(local));
        $$ = LetStmt(std::move(u_local));
    }
    | LET pattern EQ literal_expr SEMI {
        auto u_expr = std::make_unique<Expr>(Expr::makeLit(std::move($4)));
        Local local = Local::makeInit(std::move(u_expr));
        local.pat = std::make_unique<Pat>(std::move($2));
        local.type = std::nullopt;
        auto u_local = std::make_unique<Local>(std::move(local));
        $$ = LetStmt(std::move(u_local));
    }
    | LET pattern COLON type_annotation SEMI {
        Local local = Local::makeDecl();
        local.pat = std::make_unique<Pat>(std::move($2));

        auto u_path = std::make_unique<Path>($4);
        auto o_type = Type::makePath(std::move(u_path));
        local.type = std::make_unique<Type>(std::move(o_type));
        auto u_local = std::make_unique<Local>(std::move(local));
        $$ = LetStmt(std::move(u_local));
    }
    | LET pattern COLON type_annotation EQ literal_expr SEMI {
        auto u_expr = std::make_unique<Expr>(Expr::makeLit(std::move($6)));
        Local local = Local::makeInit(std::move(u_expr));
        local.pat = std::make_unique<Pat>(std::move($2));

        auto u_path = std::make_unique<Path>($4);
        auto o_type = Type::makePath(std::move(u_path));
        local.type = std::make_unique<Type>(std::move(o_type));

        auto u_local = std::make_unique<Local>(std::move(local));
        $$ = LetStmt(std::move(u_local));
    }
    ;

pattern
    : identifier_pattern        { $$ = std::move($1); }
    ;

identifier_pattern
    : identifier        {
        $$ = MRC::AST::Pat::makeIdent(BindingMode::make(), std::move($1), std::nullopt);
    }
    | MUT identifier {
        $$ = MRC::AST::Pat::makeIdent(BindingMode::makeMut(), std::move($2), std::nullopt);
    }
    ;

identifier
    : IDENTIFIER { $$ = Ident($1.symbol); }
    ;

type
    : type_annotation {
        auto u_path = std::make_unique<Path>(std::move($1));
        $$ = Type::makePath(std::move(u_path));
    }
    ;

type_annotation
    : type_path { $$ = MRC::AST::Path($1); }
    ;

type_path
    : path_segment {
        $$.push_back($1);
    }
    | type_path SCOPE_RESOLUTION path_segment {
        $$ = $1;
        $$.push_back($3);
    }
    ;

path_segment
    : IDENTIFIER {
        $$ = $1.symbol;
    }
    | TYPE {
        $$ = $1.symbol;
    }
    ;

expr
    : expr_without_block { $$ = $1 }
    | expr_with_block { $$ = $1 }
    ;

expr_without_block
    : literal_expr { $$ = MRC::AST::Expr::makeLit(std::move($1)); }
    ;

expr_with_block
    : block_expr {
        auto u_block = std::make_unique<Block>(std::move($1));
        $$ = Expr::makeBlock(std::move(u_block));
    }
    ;

block_expr
    : LBRACKET statements RBRACKET {
        $$ = Block(std::move($2));
    }
    ;

statements
    : statement {
        $$.push_back(std::move($1));
    }
    | statements statement {
        $$ = std::move($1);
        $$.push_back(std::move($2));
    }
    | statement expr_without_block {
        $$.push_back(std::move($1));
        auto u_expr = std::make_unique<Expr>(std::move($2));
        auto semi = Stmt::makeSemi(std::move(u_expr));
        $$.push_back(std::move(semi));
    }
    | statements statement expr_without_block {
        $$ = std::move($1);
        $$.push_back(std::move($2));
        auto u_expr = std::make_unique<Expr>(std::move($3));
        auto semi = Stmt::makeSemi(std::move(u_expr));
        $$.push_back(std::move(semi));
    }
    | expr_without_block {
        auto u_expr = std::make_unique<Expr>(std::move($1));
        auto semi = Stmt::makeSemi(std::move(u_expr));
        $$.push_back(std::move(semi));
    }
    ;


literal_expr
    : INTEGER_LITERAL           { $$ = MU<MRC::AST::Lit>(MRC::AST::Lit::makeInteger($1)); }
    | FLOAT_LITERAL             { $$ = MU<MRC::AST::Lit>(MRC::AST::Lit::makeFloat($1)); }
    | STR_LITERAL               { $$ = MU<MRC::AST::Lit>(MRC::AST::Lit::makeStr($1)); }
    ;

%%

namespace MRC {
    void Parser::error(const location &location, const std::string &message)
    {
        std::cerr << "Error at lines " << location << ": " << message << std::endl;
    }
}
