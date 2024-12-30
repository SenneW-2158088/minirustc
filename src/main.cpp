#include "lexer/Scanner.h"
#include "parser/parser.h"
#include "driver/Driver.h"
#include "ast/visitor/PrintVisitor.h"

#include <cstdio>

void printAst(MRC::AST::Ast* ast) {
  MRC::AST::PrintVisitor printer;
  for (auto &item : ast->items){
    printer.visit_item(item);
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
    fn b(){
        let c = 30;
    }
    fn main() -> i32 {
      let a = 30;

      let c = b();
    }
    )";
    driver.parse(method);
    // driver.parse("let a;");
    // driver.parse("let a = 30;");
    // driver.parse("let a: i32;");
    // driver.parse("let a: i32 = 32;");
    printAst(driver.ast());

    return 0;
}
