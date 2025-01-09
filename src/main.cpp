#include "ast/visitor/PrintVisitor.h"
#include "driver/Driver.h"
#include "lexer/Scanner.h"
#include "mr/visitor/PrintVisitor.h"
#include "parser/parser.h"
#include "typechecking/TypeChecker.h"
#include "interpreter/Interpreter.h"

#include "mr/Mr.h"

#include <cstdio>
#include <cwchar>

void type_check(MRC::AST::Ast *ast) {
  MRC::TS::TypeChecker typechecker{};

  MRC::AST::PrintVisitor printer{typechecker.context};
  MRC::MR::MrBuilder builder(typechecker.context);


  try {
    for (auto &body : ast->items) {
      typechecker.visit_item(body);
    }

    typechecker.context->print_context();

    // for (auto &body : ast->items) {
    //   printer.visit_item(body);
    // }

    MRC::MR::Mr mr = builder.build(*ast);
    MRC::MR::PrintVisitor mr_printer(&mr);
    mr_printer.print();
    mr.tree->print();

    MRC::INTERP::Interpreter interpreter(typechecker.context, mr);

    auto entry = std::string("main");
    interpreter.interp(entry);
    
  } catch (MRC::TS::TypeError err) {
    std::cout << err.what() << std::endl;
  }
}

int main(int argc, char *argv[]) {

  MRC::Driver driver{};
  auto method = R"(
    fn main() {
        let a = 32;
        let b = 18;
        let c = a + b;
        a + 3;
        println!(a);
    }
    )";
  driver.parse(method);
  type_check(driver.ast());

  return 0;
}
