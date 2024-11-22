#pragma once

#include <variant>
#include <vector>

#include "ast/prelude.h"

namespace MRC::AST {

using StmtPtr = U<Stmt>;
using ExprPtr = U<Expr>;

typedef int Index;

struct Ast {
  std::vector<Stmt> statements{};
  std::vector<Expr> expressions{};

public:
  Ast() = default;
  Index insert(Stmt stmt) {
    statements.push_back(std::move(stmt));
    return statements.size() - 1;
  }
  Index insert(Expr expr) {
    expressions.push_back(std::move(expr));
    return expressions.size() - 1;
  }
};

} // namespace MRC::AST
