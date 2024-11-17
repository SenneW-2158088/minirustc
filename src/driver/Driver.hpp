#pragma once

#include <fstream>
#include <istream>
#include <sstream>
#include <string>

#include "lexer/Scanner.hpp"
#include "parser/parser.h"

namespace MRC {
class Driver {
private:
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
