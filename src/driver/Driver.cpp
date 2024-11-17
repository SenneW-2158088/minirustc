#include "Driver.hpp"
#include "parser/parser.h"

#include <istream>
#include <string>

MRC::Driver::Driver() : _scanner(*this), _parser(this, &_scanner) {}

bool MRC::Driver::parse(const std::string &input) {
  std::istringstream stream{input};
  return parse(stream);
}

bool MRC::Driver::parse(std::istream &input) {
  _scanner.switch_streams(&input, nullptr);
  return _parser.parse();
}
