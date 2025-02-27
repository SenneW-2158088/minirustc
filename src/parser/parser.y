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
%token <MRC::Token> PLUS MINUS STAR SLASH PERCENT
%token <MRC::Token> EQ
%token <MRC::Token> PLUSEQ MINUSEQ STAREQ SLASHEQ PERCENTEQ
%token <MRC::Token> EQEQ NOTEQ ANDEQ OREQ
%token <MRC::Token> GT LT GTEQ LTEQ
%token <MRC::Token> NOT ANDAND OROR
%token <MRC::Token> COMMA SEMI COLON RIGHT_ARROW LEFT_ARROW
%token <MRC::Token> LPAREN RPAREN LBRACKET RBRACKET LBRACE RBRACE


%token <MRC::Token> PRINT
%token <MRC::Token> IDENTIFIER
%token <MRC::Token> TYPE
%token <MRC::Token> SCOPE_RESOLUTION

// Misc
%token ERROR    1
%token EOF      0

/* -- Statements -- */
%type <AST::Stmt>                   statement
%type <AST::Block::Statements>      statements

/* -- Expressions -- */
%type <U<AST::Expr>>                expr
%type <Opt<U<AST::Expr>>>           expr.opt
%type <std::vector<U<AST::Expr>>>   exprs
%type <U<AST::Expr>>                expr_without_block
%type <U<AST::Expr>>                expr_with_block

%type <U<AST::Lit>>                 literal_expr
%type <U<AST::Block>>               block_expr
%type <U<AST::Expr>>                loop_expr
%type <U<AST::Expr>>                call_expr
%type <U<AST::Expr>>                operator_expr
%type <U<AST::Expr>>                if_expr

/* -- Identifier -- */
%type <AST::Ident>                  identifier

/* -- Patterns -- */
%type <U<AST::Pat>>                 pattern
%type <U<AST::Pat>>                 identifier_pattern

/* -- Types -- */
%type <U<AST::Type>>                type
%type <Opt<U<AST::Type>>>           type.opt
%type <Opt<U<AST::Type>>>           typedecl.opt

%type <U<AST::Path>>                type_annotation
%type <std::vector<Symbol>>         type_path
%type <Symbol>                      path_segment


/* -- Initializers -- */
%type <U<AST::Expr>>                init
%type <Opt<U<AST::Expr>>>           init.opt
%type <U<AST::Type>>                     impl
%type <Opt<U<AST::Type>>>                impl.opt


/* -- Items -- */
%type <U<AST::Item>>              item

%type <U<AST::Fn>>                  function
%type <AST::Fn::Params>             parameters
%type <Opt<AST::Fn::Params>>        parameters.opt
%type <AST::Param>                  parameter
%type <AST::Fn::Body>               block
%type <U<AST::Local>>               local

%left OROR
%left ANDAND
%left EQEQ NOTEQ
%left LT LE GT GE
%left PLUS MINUS
%left STAR SLASH PERCENT
%right NOT
%left LPAREN RPAREN
%left SEMI

%nonassoc EQ PLUSEQ MINUSEQ STAREQ SLASHEQ PERCENTEQ
%nonassoc ELSE

%verbose
%define parse.trace
%define parse.error verbose

%left PRINT

%%
program
    : items         { }
    | EOF           { }
    ;

items
    : item         {
        ast->insert(std::move(*$1));
    }
    | items item   {
        ast->insert(std::move(*$2));
    }
    ;

statements
    :
    | statement {
        $$.push_back(std::move($1));
    }
    | statements statement {
        $$ = std::move($1);
        $$.push_back(std::move($2));
    }
    ;

item
    : FN identifier function {
        $$ = MU<Item>(Item::makeFn(ast->getId(), std::move($2), std::move($3)));
    }
    ;

function
    : LPAREN parameters RPAREN impl.opt block {
        $$ = MU<Fn>(Fn(std::move($2), std::move($5), std::move($4)));
    }
    ;

parameters
    :
    | parameter {
        $$.push_back(std::move($1));
    }
    | parameters COMMA parameter {
        $$ = std::move($1);
        $$.push_back(std::move($3));
    }
    ;

parameters.opt
    : { $$ = std::nullopt; }
    | parameters { $$ = std::move($1); }
    ;

parameter
    : pattern COLON type {
        $$ = Param(ast->getId(), std::move($3), std::move($1));
    }
    ;

block
    : block_expr {
        $$ = std::optional(std::move($1));
    }
    | SEMI { $$ = std::nullopt; }
    ;

/* Statements */
statement
    : item                    { $$ = Stmt::makeItem(ast->getId(), std::move($1)); }
    | LET local SEMI          { $$ = Stmt::makeLet(ast->getId(), std::move($2)); }
    | PRINT LPAREN expr_without_block RPAREN SEMI { $$ = Stmt::makePrint(ast->getId(), std::move($3)); }
    | expr_with_block         { $$ = Stmt::makeExpr(ast->getId(), std::move($1)); }
    | expr_without_block SEMI { $$ = Stmt::makeExpr(ast->getId(), std::move($1)); }
    ;

local
    : pattern typedecl.opt SEMI { $$ = MU<Local>(Local::makeDecl(ast->getId(), std::move($1), std::move($2))); }
    | pattern typedecl.opt init { $$ = MU<Local>(Local::makeInit(ast->getId(), std::move($1), std::move($2), std::move($3))); }
    ;

pattern
    : identifier_pattern        { $$ = std::move($1); }
    ;

identifier_pattern
    : identifier        {
      $$ = MU<Pat>(Pat::makeIdent(BindingMode::make(), std::move($1), std::nullopt));
    }
    | MUT identifier {
        $$ = MU<Pat>(Pat::makeIdent(BindingMode::makeMut(), std::move($2), std::nullopt));
    }
    ;

identifier
    : IDENTIFIER { $$ = Ident($1.symbol); }
    ;

init
    : EQ expr { $$ = std::move($2); }
    ;

init.opt
    : { $$ = std::nullopt; }
    | init { $$ = std::move($1); }
    ;

impl
    : RIGHT_ARROW type { $$ = std::move($2); }
    ;

impl.opt
    : { $$ = std::nullopt; }
    | RIGHT_ARROW type { $$ = std::move($2); }
    ;

typedecl.opt
    : { $$ = std::nullopt; }
    | COLON type { $$ = std::move($2); }
    ;

type
    : type_annotation {
        $$ = MU<Type>(Type::makePath(std::move($1)));
    }
    ;

type.opt
    : { $$ = std::nullopt; }
    | type {
      $$ = std::move($1);
    }
    ;

type_annotation
    : type_path { $$ = MU<Path>(Path(std::move($1))); }
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



/* Expressions */
expr
  : expr_without_block { $$ = std::move($1); }
  | expr_with_block { $$ = std::move($1); }
  ;

expr.opt
    : { $$ = std::nullopt; }
    | expr { $$ = std::move($1); }
    ;

exprs
    :
    | expr {
        $$.push_back(std::move($1));
    }
    | exprs COMMA expr {
        $$ = std::move($1);
        $$.push_back(std::move($3));
    }
    ;

expr_without_block
  : literal_expr { $$ = MU<Expr>(Expr::makeLit(ast->getId(), std::move($1))); }
  | type_annotation { $$ = MU<Expr>(Expr::makePath(ast->getId(), std::move($1))); }
  | expr LPAREN exprs RPAREN { $$ = MU<Expr>(Expr::makeCall(ast->getId(), std::move($1), std::move($3))); }
  | operator_expr { $$ = std::move($1); }
  | BREAK expr.opt    { $$ = MU<Expr>(Expr::makeBreak(ast->getId(), std::move($2))); }
  | CONTINUE      { $$ = MU<Expr>(Expr::makeContinue(ast->getId())); }
  | RETURN expr.opt   { $$ = MU<Expr>(Expr::makeReturn(ast->getId(), std::move($2))); }
  ;

expr_with_block
  : block_expr {
      $$ = MU<Expr>(Expr::makeBlock(ast->getId(), std::move($1)));
  }
  | loop_expr {
    $$ = std::move($1);
    }
  | if_expr { $$ = std::move($1); }
  ;

block_expr
  : LBRACKET statements RBRACKET {
      $$ = MU<Block>(Block(std::move($2)));
  }
  ;

if_expr
    : IF expr block_expr { $$ = MU<Expr>(Expr::makeIf(ast->getId(), std::move($2), std::move($3))); }
    | IF expr block_expr ELSE block_expr {
        auto b_expr = MU<Expr>(Expr::makeBlock(ast->getId(), std::move($5)));
        $$ = MU<Expr>(Expr::makeIfElse(ast->getId(), std::move($2), std::move($3), std::move(b_expr)));
    }
    | IF expr block_expr ELSE if_expr {
        $$ = MU<Expr>(Expr::makeIfElse(ast->getId(), std::move($2), std::move($3), std::move($5)));
    }
    ;

loop_expr
    : LOOP block_expr {
        $$ = MU<Expr>(Expr::makeLoop(ast->getId(), std::move($2)));
    }
    | WHILE expr block_expr {
        $$ = MU<Expr>(Expr::makeWhile(ast->getId(), std::move($2), std::move($3)));
    }
    ;

literal_expr
    : STR_LITERAL               { $$ = MU<Lit>(Lit::makeStr(ast->getId(),$1)); }
    | INTEGER_LITERAL           { $$ = MU<Lit>(Lit::makeInteger(ast->getId(),$1)); }
    | FLOAT_LITERAL             { $$ = MU<Lit>(Lit::makeFloat(ast->getId(),$1)); }
    | BOOLEAN_LITERAL           { $$ = MU<Lit>(Lit::makeBoolean(ast->getId(),$1)); }
    ;

operator_expr
    : expr PLUS expr            { $$ = MU<Expr>(Expr::makeBinary(ast->getId(), BinOp::makeAdd(), std::move($1), std::move($3))); }
    | expr MINUS expr           { $$ = MU<Expr>(Expr::makeBinary(ast->getId(), BinOp::makeSub(), std::move($1), std::move($3))); }
    | expr STAR expr            { $$ = MU<Expr>(Expr::makeBinary(ast->getId(), BinOp::makeMul(), std::move($1), std::move($3))); }
    | expr SLASH expr           { $$ = MU<Expr>(Expr::makeBinary(ast->getId(), BinOp::makeDiv(), std::move($1), std::move($3))); }
    | expr PERCENT expr         { $$ = MU<Expr>(Expr::makeBinary(ast->getId(), BinOp::makeMod(), std::move($1), std::move($3))); }

    | expr EQ expr              { $$ = MU<Expr>(Expr::makeAssign(ast->getId(), std::move($1), std::move($3))); }
    | expr PLUSEQ expr          { $$ = MU<Expr>(Expr::makeAssignOp(ast->getId(), BinOp::makeAdd(), std::move($1), std::move($3))); }
    | expr MINUSEQ expr         { $$ = MU<Expr>(Expr::makeAssignOp(ast->getId(), BinOp::makeSub(), std::move($1), std::move($3))); }
    | expr STAREQ expr          { $$ = MU<Expr>(Expr::makeAssignOp(ast->getId(), BinOp::makeMul(), std::move($1), std::move($3))); }
    | expr SLASHEQ expr         { $$ = MU<Expr>(Expr::makeAssignOp(ast->getId(), BinOp::makeDiv(), std::move($1), std::move($3))); }
    | expr PERCENTEQ expr       { $$ = MU<Expr>(Expr::makeAssignOp(ast->getId(), BinOp::makeMod(), std::move($1), std::move($3))); }

    | expr EQEQ expr            { $$ = MU<Expr>(Expr::makeBinary(ast->getId(), BinOp::makeEq(), std::move($1), std::move($3))); }
    | expr NOTEQ expr           { $$ = MU<Expr>(Expr::makeBinary(ast->getId(), BinOp::makeNe(), std::move($1), std::move($3))); }
    | expr GT expr              { $$ = MU<Expr>(Expr::makeBinary(ast->getId(), BinOp::makeGt(), std::move($1), std::move($3))); }
    | expr LT expr              { $$ = MU<Expr>(Expr::makeBinary(ast->getId(), BinOp::makeLt(), std::move($1), std::move($3))); }
    | expr GTEQ expr            { $$ = MU<Expr>(Expr::makeBinary(ast->getId(), BinOp::makeGe(), std::move($1), std::move($3))); }
    | expr LTEQ expr            { $$ = MU<Expr>(Expr::makeBinary(ast->getId(), BinOp::makeLe(), std::move($1), std::move($3))); }
    | expr ANDAND expr          { $$ = MU<Expr>(Expr::makeBinary(ast->getId(), BinOp::makeAnd(), std::move($1), std::move($3))); }
    | expr OROR expr            { $$ = MU<Expr>(Expr::makeBinary(ast->getId(), BinOp::makeOr(), std::move($1), std::move($3))); }


    | NOT expr                  { $$ = MU<Expr>(Expr::makeUnary(ast->getId(), UnOp::makeNot(), std::move($2))); }
    ;
%%

namespace MRC {
    void Parser::error(const location &location, const std::string &message)
    {
        std::cerr << "Error at lines " << driver->location << ": " << message << std::endl;
    }
}
