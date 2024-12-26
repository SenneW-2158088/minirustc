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
    driver.parse("let a = 30;");
    printAst(driver.ast());
    // driver.parse("let a = 30u16;");
    // driver.parse("let a = 30_u16;");
    // driver.parse("let a = 30_u16;");
    // driver.parse("fn main();");
    // driver.parse("fn main(hello: i32);");
    // driver.parse("fn main() -> i32;");
    // auto method = R"(
    // fn main() -> i32 {
    //   let a = 30;
    //   fn a() {
    //     let b = 20;
    //   }
    // }
    // )";
    // driver.parse(method);
    // driver.parse("let a;");
    // driver.parse("let a = 30;");
    // driver.parse("let a: i32;");
    // driver.parse("let a: i32 = 32;");
    // std::cout << "pass" << std::endl;

    return 0;
}
