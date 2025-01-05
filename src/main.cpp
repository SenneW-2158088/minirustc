#include "lexer/Scanner.h"
#include "parser/parser.h"
#include "driver/Driver.h"
#include "ast/visitor/PrintVisitor.h"
#include "typechecking/TypeChecker.h"

#include <cstdio>
#include <cwchar>

void printAst(MRC::AST::Ast* ast) {
  MRC::AST::PrintVisitor printer;
  for (auto &body: ast->bodies){
    printer.visit_body(body);
  }
}

void type_check(MRC::AST::Ast *ast) {
    MRC::TS::TypeChecker typechecker;

    try {
        for (auto &body: ast->bodies){
          typechecker.visit_body(body);
        }
        typechecker.context.print_context();
    }catch(MRC::TS::TypeError err) {
        std::cout << err.what() << std::endl;
    }

}

int main(int argc, char* argv[]) {

    // read options
    MRC::Driver driver{};
    auto method = R"(
    fn main() {
        let a = 3;
        let b = 5;
        let x: i64 = a + b;
        let c: i32 = a;
    }
    )";
    driver.parse(method);
    type_check(driver.ast());

    printAst(driver.ast());

    return 0;
}
