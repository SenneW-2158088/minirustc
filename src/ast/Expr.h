#pragma once

#include "ast/Lit.h"
#include "struct/Pat.h"
#include "util/util.h"

#include <variant>

namespace MRC::AST {

struct Pat;  // Forward declare
struct Expr; // Forward declare

/* Literal expression */
struct LitExpr {
  U<Lit> lit;

public:
  LitExpr() = default;
  explicit LitExpr(U<Lit> lit) : lit(std::move(lit)) {}
};

/* Expression expression */
struct ExprExpr {
  U<Expr> expr;

public:
  ExprExpr() = default;
  explicit ExprExpr(U<Expr> expr) : expr(std::move(expr)) {}
};

/* Let expression */
struct LetExpr {
  U<Pat> pattern;
  U<Expr> expr;

public:
  LetExpr() = default;
  explicit LetExpr(U<Pat> pattern, U<Expr> expr)
      : pattern(std::move(pattern)), expr(std::move(expr)) {}
};

struct Expr {
  using ExprKind = std::variant<LitExpr, ExprExpr, LetExpr>;
  ExprKind kind;

public:
  Expr() : kind() {}

  explicit Expr(ExprKind kind) : kind(std::move(kind)) {}

  static Expr makeLit(U<Lit> lit) { return Expr(LitExpr(std::move(lit))); }
  static Expr makeExpr(U<Expr> expr) { return Expr(ExprExpr(std::move(expr))); }
  static Expr makeLet(U<Pat> pattern, U<Expr> expr) {
    return Expr(LetExpr(std::move(pattern), std::move(expr)));
  }
};

} // namespace MRC::AST
