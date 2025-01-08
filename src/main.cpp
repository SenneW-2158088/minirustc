#include "ast/visitor/PrintVisitor.h"
#include "driver/Driver.h"
#include "lexer/Scanner.h"
#include "parser/parser.h"
#include "typechecking/TypeChecker.h"

#include <cstdio>
#include <cwchar>

void type_check(MRC::AST::Ast *ast) {
  MRC::TS::TypeChecker typechecker;
  MRC::AST::PrintVisitor printer{&typechecker.context};

  try {
    for (auto &body : ast->items) {
      typechecker.visit_item(body);
    }

    typechecker.context.print_context();

    for (auto &body : ast->items) {
      printer.visit_item(body);
    }
    
  } catch (MRC::TS::TypeError err) {
    std::cout << err.what() << std::endl;
  }
}

int main(int argc, char *argv[]) {

  MRC::Driver driver{};
  auto method = R"(
    fn test(a: i32) {
      let b = a;
    }
    

    fn main() {
        let a = 32;
        let d = test(a);
    }
    )";
  driver.parse(method);
  type_check(driver.ast());

  return 0;
}
