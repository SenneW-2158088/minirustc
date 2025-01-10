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
#include <fstream>

std::string read_file(std::string &filename) {
    std::ifstream input_file(filename);
    if (!input_file.is_open()) {
        std::cerr << "Error: Could not open file " << filename << "\n";
        exit(1);
    }

    std::string code((std::istreambuf_iterator<char>(input_file)),
                    std::istreambuf_iterator<char>());
    input_file.close();
    return code;
}

U<MRC::AST::Ast> parse_code(const std::string& code) {
    MRC::Driver driver{};
    driver.parse(code);
    return std::move(driver.get());
}

void type_check_ast(MRC::AST::Ast* ast, MRC::TS::TypeChecker& typechecker) {
    for (auto& item : ast->items) {
        typechecker.visit_item(item);
    }
    // Uncomment to debug type checking
    // typechecker.context->print_context();
}

MRC::MR::Mr build_mr(MRC::AST::Ast* ast, MRC::TS::TypeChecker& typechecker) {
    MRC::MR::MrBuilder builder(typechecker.context);
    MRC::MR::Mr mr = builder.build(*ast);
    return mr;
}

void interpret_mr(MRC::MR::Mr& mr, MRC::TS::TypeChecker& typechecker, std::string& entry_point) {
    MRC::INTERP::Interpreter interpreter(typechecker.context, mr);
    interpreter.interp(entry_point);
}

void process_program(const std::string& code, bool verbose, std::string entry) {
    try {
        P<MRC::AST::Ast> ast = parse_code(code);

        MRC::TS::TypeChecker typechecker{};
        type_check_ast(ast.get(), typechecker);

        MRC::AST::PrintVisitor printer{typechecker.context};
        // if(verbose) {
        //   for(auto &item : ast->items) {
        //     printer.visit_item(item);
        //   }
        // }

        MRC::MR::Mr mr = build_mr(ast.get(), typechecker);
        if(verbose) {
          MRC::MR::PrintVisitor mr_printer(&mr);
          mr_printer.print();
          mr.tree->print();
        }

        interpret_mr(mr, typechecker, entry);

    } catch (const MRC::TS::TypeError& err) {
        std::cerr << "Type Error: " << err.what() << std::endl;
        exit(1);
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        exit(1);
    }
}

void print_usage() {
  std::cout << "Usage: mrc" << " [-v] [-e entry] <input_file>\n";
  exit(-1);
}

int main(int argc, char *argv[]) {
  // Flags
  std::string entry = "main"; // Default entry
  bool verbose = false;
  std::string filename;

  if(argc < 2) print_usage();
  for (int i = 1; i < argc; i++) {
      std::string arg = argv[i];
      if (arg == "-v") {
          verbose = true;
          continue;
      }
      if (arg == "-e" && i + 1 < argc) {
          entry = argv[++i];
          continue;
      }
      if (filename.empty()) {
          filename = arg;
      }
  }

  if (filename.empty()) {
    print_usage();
  }

  std::string program = read_file(filename);
  process_program(program, verbose, entry);

  return 0;
}
