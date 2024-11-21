#include "lexer/Scanner.h"
#include "parser/parser.h"
#include "driver/Driver.h"
#include "ast/visitor/PrintVisitor.h"

#include <cstdio>

void printAst(MRC::AST::Ast* ast) {
  MRC::AST::PrintVisitor printer;
  for (auto &stmt : ast->statements){
    printer.visit_stmt(stmt);
  }
}

int main(int argc, char* argv[]) {

    // read options
    MRC::Driver driver{};

    driver.parse("let a;");
    driver.parse("let a = 30;");
    driver.parse("let a: i32;");
    driver.parse("let a: i32 = 32;");
    printAst(driver.ast());

    return 0;
}
