#pragma once

#include <variant>
#include <vector>

#include "ast/prelude.h"
#include "tst/Type.h"
#include "tst/tst.h"
#include "typechecking/Type.h"
#include "util/util.h"

namespace MRC::AST {

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

struct PathExpr {
  U<Path> path;
  PathExpr() = default;
  PathExpr(U<Path> path) : path(std::move(path)) {}
};

struct CallExpr {
  U<Expr> expr;
  std::vector<U<Expr>> params;
  CallExpr() = default;
  CallExpr(U<Expr> expr, std::vector<U<Expr>> params)
      : expr(std::move(expr)), params(std::move(params)) {}
};

struct BinaryExpr {
  BinOp op;
  U<Expr> first;
  U<Expr> second;
};

struct AssignExpr {
  U<Expr> first;
  U<Expr> second;
};

struct AssignOpExpr {
  BinOp op;
  U<Expr> first;
  U<Expr> second;
};

struct Expr {
  using ExprKind = std::variant<LitExpr, ExprExpr, LetExpr, BlockExpr,
                                WhileExpr, IfExpr, LoopExpr, CallExpr, PathExpr,
                                BinaryExpr, AssignExpr, AssignOpExpr>;
  ExprKind kind;
  TS::CheckType type;

public:
  Expr() : kind(), type(TS::CheckType::makeVar(TS::Type::MakeUnset())) {}

  explicit Expr(ExprKind kind)
      : kind(std::move(kind)),
        type(TS::CheckType::makeVar(TS::Type::MakeUnset())) {}

  static Expr makeCall(U<Expr> expr, std::vector<U<Expr>> params) {
    return Expr(CallExpr(std::move(expr), std::move(params)));
  }
  static Expr makeLit(U<Lit> lit) { return Expr(LitExpr(std::move(lit))); }
  static Expr makeExpr(U<Expr> expr) { return Expr(ExprExpr(std::move(expr))); }
  static Expr makeLet(U<Pat> pattern, U<Expr> expr) {
    return Expr(LetExpr(std::move(pattern), std::move(expr)));
  }
  static Expr makePath(U<Path> path) { return Expr(PathExpr(std::move(path))); }
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
  static Expr makeBinary(BinOp op, U<Expr> first, U<Expr> second) {
    return Expr(BinaryExpr(std::move(op), std::move(first), std::move(second)));
  }

  static Expr makeAssign(U<Expr> first, U<Expr> second) {
    return Expr(AssignExpr(std::move(first), std::move(second)));
  }

  static Expr makeAssignOp(BinOp op, U<Expr> first, U<Expr> second) {
    return Expr(
        AssignOpExpr(std::move(op), std::move(first), std::move(second)));
  }
};
} // namespace MRC::AST
