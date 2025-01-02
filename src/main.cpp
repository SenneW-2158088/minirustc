#include "lexer/Scanner.h"
#include "parser/parser.h"
#include "driver/Driver.h"
#include "ast/visitor/PrintVisitor.h"
#include "typechecking/TypeChecker.h"

#include <cstdio>
#include <cwchar>

void printAst(MRC::AST::Ast* ast) {
  MRC::AST::PrintVisitor printer;
  for (auto &item : ast->items){
    printer.visit_item(item);
  }
}

void type_check(MRC::AST::Ast *ast) {
    MRC::TS::TypeChecker typechecker;
    for (auto &item : ast->items){
      typechecker.visit_item(item);
    }
}

int main(int argc, char* argv[]) {

    // read options
    MRC::Driver driver{};
    // driver.parse("let a = 30;");
    // driver.parse("let a = 30u16;");
    // driver.parse("let a = 30_u16;");
    // driver.parse("let a = 30_u16;");
    // driver.parse("fn main();");
    // driver.parse("fn main(hello: i32);");
    // driver.parse("fn main() -> i32;");
    auto method = R"(
    fn main() {
      let a: i8 = 30u8;
      let b = 30_u16;
      let c = 29.0f64;
    }
    )";
    driver.parse(method);
    // driver.parse("let a;");
    // driver.parse("let a = 30;");
    // driver.parse("let a: i32;");
    // driver.parse("let a: i32 = 32;");
    type_check(driver.ast());

    printAst(driver.ast());

    return 0;
}
