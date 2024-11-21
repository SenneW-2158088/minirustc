#pragma once

#include "ast/prelude.h"

#include <vector>

namespace MRC::AST {
struct Block {
  using Statements = std::vector<Stmt>;
  Statements statements;

public:
  Block() = default;
  explicit Block(Statements statements) : statements(std::move(statements)) {}
};
} // namespace MRC::AST
