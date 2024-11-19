#pragma once

#include "ast/Lit.h"
#include "struct/Pat.h"
#include "util/util.h"

#include <variant>

namespace MRC::AST {

struct Expr; // Forward declare

/* Literal expression */
struct LiteralExpr {
  U<Lit> lit;

public:
  LiteralExpr() = default;
  explicit LiteralExpr(U<Lit> lit) : lit(std::move(lit)) {}
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
  using ExprKind = std::variant<LiteralExpr, ExprExpr, LetExpr>;
  ExprKind kind;

public:
  Expr() : kind() {}

  explicit Expr(ExprKind kind) : kind(std::move(kind)) {}

  static Expr makeLit(U<Lit> lit) { return Expr(LiteralExpr(std::move(lit))); }
  static Expr makeExpr(U<Expr> expr) { return Expr(ExprExpr(std::move(expr))); }
  static Expr makeLet (U<Pat> pattern, U<Expr> expr) { return Expr(LetExpr(std::move(pattern), std::move(expr))); }
};

} // namespace MRC::AST
