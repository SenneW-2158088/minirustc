#pragma once

#include <vector>
#include "lexer/Token.h"

namespace MRC::MR{
  struct Path {
    using PathSegments = std::vector<Symbol>;
    PathSegments segments;
    public:

    Path() = default;
    Path(Symbol segment) : segments({std::move(segment)}) {};
    Path(PathSegments segments) : segments(std::move(segments)){};

    std::string to_string() const {
      std::string full = segments[0];
      for(int i = 1; i < segments.size(); ++i) {
        full += ":" + segments[i];
      }

      return full;
    }
  };
}
