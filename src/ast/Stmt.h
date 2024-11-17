#pragma once

#include "ast/Expr.h"
#include "util/util.h"

#include <variant>

namespace MRC::AST {

struct Stmt;

struct ExprStmt {
  U<Expr> expr;
};

struct Stmt {
  std::variant<ExprStmt> kind{};

public:
  Stmt() = default;
};
} // namespace MRC::AST
