#pragma once

#include <vector>

#include "Expr.h"
#include "Stmt.h"
#include "util/util.h"

namespace MRC::AST {

using StmtPtr = U<Stmt>;
using ExprPtr = U<Expr>;

class Ast {
  std::vector<StmtPtr> statements{};
  std::vector<ExprPtr> expressions{};

public:
  Ast() = default;
  void insert(StmtPtr stmt) { statements.push_back(std::move(stmt)); }
  void insert(ExprPtr expr) { expressions.push_back(std::move(expr)); }
};

} // namespace MRC::AST
