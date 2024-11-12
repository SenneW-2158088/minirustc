%require "3.2"
// %language "c++"

%{
#include "lexer/lexer.h"
void yyerror(const char* str);
void yylex(int* str);
%}

// Token declarations
%token TOKEN1 TOKEN2 // Add your token declarations here

// Precedence declarations (if needed)
%left '+' '-'
%left '*' '/'

%defines

%%

// Grammar rules
start: /* empty */
     | start expression
     ;

expression: TOKEN1
          | TOKEN2
          // Add your grammar rules here
          ;

%%

void yyerror(const char *s)
{
    // Error handling code
    fprintf(stderr, "Error: %s\n", s);
}

// You can add additional C code here if needed
