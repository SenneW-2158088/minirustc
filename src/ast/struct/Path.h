#pragma once

#include "Token.h"
namespace MRC::AST {
  struct Path {
    using PathSegments = std::vector<Symbol>;
    PathSegments segments;
    public:
    Path() = default;
    Path(Symbol segment) : segments({std::move(segment)}) {};
    Path(PathSegments segments) : segments(std::move(segments)){};
  };
}
