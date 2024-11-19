#pragma once

#include "ast/Lit.h"
#include "util/util.h"

#include <variant>

namespace MRC::AST {

struct Expr; // Forward declare

struct LiteralExpr {
  U<Lit> lit;

public:
  LiteralExpr() = default;
  explicit LiteralExpr(U<Lit> lit) : lit(std::move(lit)) {}
};

struct ExprExpr {
  U<Expr> expr;

public:
  ExprExpr() = default;
  explicit ExprExpr(U<Expr> expr) : expr(std::move(expr)) {}
};

struct Expr {
  using ExprKind = std::variant<LiteralExpr, ExprExpr>;
  ExprKind kind;

public:
  Expr() : kind() {}

  explicit Expr(ExprKind kind) : kind(std::move(kind)) {}

  static Expr makeLit(U<Lit> lit) { return Expr(LiteralExpr(std::move(lit))); }
  static Expr makeExpr(U<Expr> expr) { return Expr(ExprExpr(std::move(expr))); }
};

} // namespace MRC::AST
