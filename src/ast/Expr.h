#pragma once

#include "ast/prelude.h"
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

struct BlockExpr {
  U<Block> block;
  BlockExpr() = default;
  BlockExpr(U<Block> block) : block(std::move(block)) {}
};

struct IfExpr {
  using ElseExpr = std::optional<U<Expr>>;
  U<Expr> expr;
  U<Block> block;
  ElseExpr elseExpr;
  IfExpr() = default;
  IfExpr(U<Expr> expr, U<Block> block, ElseExpr elseExpr)
      : expr(std::move(expr)), block(std::move(block)),
        elseExpr(std::move(elseExpr)) {}
};

struct LoopExpr {
  U<Block> block;
  LoopExpr() = default;
  LoopExpr(U<Block> block) : block(std::move(block)) {}
};

struct WhileExpr {
  U<Expr> expr;
  U<Block> block;
  WhileExpr() = default;
  WhileExpr(U<Expr> expr, U<Block> block)
      : expr(std::move(expr)), block(std::move(block)) {}
};

struct Expr {
  using ExprKind = std::variant<LitExpr, ExprExpr, LetExpr, BlockExpr,
                                WhileExpr, IfExpr, LoopExpr>;
  ExprKind kind;

public:
  Expr() : kind() {}

  explicit Expr(ExprKind kind) : kind(std::move(kind)) {}

  static Expr makeLit(U<Lit> lit) { return Expr(LitExpr(std::move(lit))); }
  static Expr makeExpr(U<Expr> expr) { return Expr(ExprExpr(std::move(expr))); }
  static Expr makeLet(U<Pat> pattern, U<Expr> expr) {
    return Expr(LetExpr(std::move(pattern), std::move(expr)));
  }
  static Expr makeIf(U<Expr> condition, U<Block> thenBlock) {
    return Expr(
        IfExpr(std::move(condition), std::move(thenBlock), std::nullopt));
  }

  static Expr makeIfElse(U<Expr> condition, U<Block> thenBlock,
                         U<Expr> elseExpr) {
    return Expr(IfExpr(std::move(condition), std::move(thenBlock),
                       std::move(elseExpr)));
  }

  static Expr makeLoop(U<Block> block) {
    return Expr(LoopExpr(std::move(block)));
  }

  static Expr makeWhile(U<Expr> condition, U<Block> block) {
    return Expr(WhileExpr(std::move(condition), std::move(block)));
  }
  static Expr makeBlock(U<Block> block) {
    return Expr(BlockExpr(std::move(block)));
  }
};

} // namespace MRC::AST
