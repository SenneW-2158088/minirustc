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
  using StmtKind = std::variant<ExprStmt>;
  StmtKind kind{};

public:
  Stmt() = default;
  explicit Stmt(StmtKind kind) : kind(std::move(kind)) {}

  static Stmt makeExpr(U<Expr> expr) { return Stmt(ExprStmt(std::move(expr))); }
};
} // namespace MRC::AST
