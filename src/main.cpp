#include <iostream>
#include "lexer/lexer.h"
#include "parser/parser.h"

extern int yyparse();

int main(void){
  std::cout << "Hello World!" << std::endl;
  yyparse();
  return 0;
}
