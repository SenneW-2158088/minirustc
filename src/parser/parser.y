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
        struct Block;
        struct Ident;
        struct Local;
        struct Pat;
        struct Path;
        struct Type;
    }
}

%code top {
    #include <iostream>
    #include <memory>
    #include "driver/Driver.h"
    #include "lexer/Scanner.h"
    #include "parser/parser.h"
    #include "lexer/Token.h"

    #include "ast/struct/Path.h"
    #include "ast/struct/Block.h"
    #include "ast/struct/Pat.h"
    #include "ast/struct/Local.h"

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

%token <MRC::Token> IDENTIFIER
%token <MRC::Token> TYPE
%token <MRC::Token> SCOPE_RESOLUTION

// Misc
%token ERROR    1
%token EOF      0

/* Defining Types */

// Statements
%type <MRC::AST::Stmt>              statement
%type <MRC::AST::LetStmt>           let_statement

// Expressions
%type <MRC::AST::Expr>              expr
%type <U<MRC::AST::Lit>>            literal_expr

// Patterns
%type <MRC::AST::Pat>               pattern
%type <MRC::AST::Pat>               identifier_pattern

// Path
%type <MRC::AST::Path>              type_annotation
%type <std::vector<MRC::Symbol>>    type_path
%type <MRC::Symbol>                 path_segment

%%

program
    : statement program     { ast->insert(std::move($1));
    }
    | EOF                   { }
    ;

/* Statements */
statement
    : let_statement {
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
    : IDENTIFIER        {
        $$ = MRC::AST::Pat::makeIdent(BindingMode::make(), Ident($1.symbol), std::nullopt);
    }
    | MUT IDENTIFIER    { }
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
    : literal_expr { $$ = MRC::AST::Expr::makeLit(std::move($1)); }
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
