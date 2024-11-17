#pragma once

#include <variant>

namespace MRC::AST {

struct IntegerLit {
  int value;
};

struct Lit {
  std::variant<IntegerLit> kind{};

public:
  Lit() = default;
};
} // namespace MRC::AST
