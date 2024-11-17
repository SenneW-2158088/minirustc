#pragma once

#include "lexer/Scanner.h"
#include "parser/parser.h"

#include "ast/Ast.h"
#include "util/util.h"

#include <istream>
#include <sstream>
#include <string>

namespace MRC {
class Driver {
private:
  U<AST::Ast> _ast;
  Scanner _scanner;
  Parser _parser;

public:
  Parser::location_type location{};
  Driver();
  bool parse(const std::string &input);
  bool parse(std::istream &input);

  void update_location() { std::cout << "update location" << std::endl; };
};
} // namespace MRC
