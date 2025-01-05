#pragma once

#include "ast/Stmt.h"
#include "ast/prelude.h"

#include <variant>
#include <vector>

namespace MRC::AST {
struct Block {
  using Statements = std::vector<Stmt>;
  Statements statements;

public:
  Block() = default;
  explicit Block(Statements statements) : statements(std::move(statements)) {}

  Opt<Expr*> expr() {
    if(statements.size() == 0) { return std::nullopt; }
    auto &l_expr = statements.back();
    if(std::holds_alternative<ExprStmt>(l_expr.kind)) {
      auto &ref = std::get<ExprStmt>(l_expr.kind);
      // TODO: Can cause bugs
      return std::optional(ref.expr.get());
    }

    return std::nullopt;
  }
};
} // namespace MRC::AST
