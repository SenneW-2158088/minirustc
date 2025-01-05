#include "ast/visitor/PrintVisitor.h"
#include "driver/Driver.h"
#include "lexer/Scanner.h"
#include "parser/parser.h"
#include "typechecking/TypeChecker.h"

#include <cstdio>
#include <cwchar>

void printAst(MRC::AST::Ast *ast) {
  MRC::AST::PrintVisitor printer;
  for (auto &body : ast->items) {
    printer.visit_item(body);
  }
}

void type_check(MRC::AST::Ast *ast) {
  MRC::TS::TypeChecker typechecker;

  try {
    for (auto &body : ast->items) {
      typechecker.visit_item(body);
    }
    typechecker.context.print_context();
  } catch (MRC::TS::TypeError err) {
    std::cout << err.what() << std::endl;
  }
}

int main(int argc, char *argv[]) {

  MRC::Driver driver{};
  auto method = R"(

    fn main() {
        let a = 3;
        let b = 5;
        let g: i32 = b;
        let x: i64 = a + b;
    }
    )";
  driver.parse(method);
  type_check(driver.ast());

  printAst(driver.ast());

  return 0;
}
