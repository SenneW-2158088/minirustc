#pragma once

#include <vector>

#include "Expr.h"
#include "Stmt.h"
#include "Lit.h"
#include "util/util.h"

namespace MRC::AST {

using StmtPtr = U<Stmt>;
using ExprPtr = U<Expr>;

typedef int Index;

class Ast {
  std::vector<StmtPtr> statements{};
  std::vector<ExprPtr> expressions{};

public:
  Ast() = default;
  Index insert(StmtPtr stmt) { statements.push_back(std::move(stmt)); return statements.size() - 1; }
  Index insert(ExprPtr expr) { expressions.push_back(std::move(expr)); return expressions.size() - 1; }
};

} // namespace MRC::AST
