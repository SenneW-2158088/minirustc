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

  U<AST::Ast> get() { return std::move(_ast); }
  void update_location(unsigned int line, unsigned int col) {
      location.begin.line = line;
      location.begin.column = col;
      location.end = location.begin;
  }
};
} // namespace MRC
