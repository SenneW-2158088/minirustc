#pragma once

#include <fstream>
#include <istream>
#include <sstream>
#include <string>

#include "parser/parser.h"

namespace MRC {
class Driver {
   private:
    Parser::location_type location;

   public:
    bool parse(const std::string& input);
    bool parse(const std::istream& input);
};
}  // namespace MRC
