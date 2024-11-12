#include "lexer/lexer.h"
#include "lexer/tokens.h"
#include "parser/parser.h"

#include <iostream>

extern int yyparse();
extern int yylex();

/* Array with tokens such that index = tokenid - 250*/
char *tokens[] = {
    "IDENTIFIER", "PRINTVAR",  "PRINTSTRING", "DEC_LITERAL", "TRUE",   "FALSE",
    "FN",         "LPAREN",    "RPAREN",      "ARROW",       "LBRACE", "RBRACE",
    "SEMICOLON",  "COLON",     "LET",         "EQ",          "PLUS",   "MINUS",
    "STAR",       "SLASH",     "PLUSEQ",      "MINUSEQ",     "ANDAND", "OROR",
    "NOT",        "GT",        "GE",          "LT",          "LE",     "EQEQ",
    "NE",         "AMPERSAND", "IF",          "ELSE",        "WHILE",  "MUT",
    "COMMA"};

int main(int argc, char *argv[]) {
  int tokenid;

  /* If we do not explicitly bind yyin to a file, stdin is assumed. */
  while ((tokenid = yylex())) {
    /* Token codes start from 250 */

    std::cout << "Found: " << tokens[tokenid - 250] << std::endl;

    /* Append value */
    if ((tokenid == IDENTIFIER) || (tokenid == DEC_LITERAL) ||
        (tokenid == PRINTSTRING) || (tokenid == PRINTVAR))

    std::cout << "= " << yytext << std::endl;
  }
  return 0;
}
