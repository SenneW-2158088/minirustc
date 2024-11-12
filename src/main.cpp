#include <iostream>
#include "lexer/lexer.h"
#include "parser/parser.h"

extern int yyparse();
extern int yylex();


int main(void){
  std::cout << "Hello World!" << std::endl;
  return 0;
}
