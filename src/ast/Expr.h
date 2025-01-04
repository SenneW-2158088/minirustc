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
  Id id;

public:
  Expr() : kind(), type(TS::CheckType::makeVar(TS::Type::MakeUnset())) {}

  explicit Expr(Id id, ExprKind kind)
      : kind(std::move(kind))
      , id(id)
      , type(TS::CheckType::makeVar(TS::Type::MakeUnset())) {}

  static Expr makeCall(Id id, U<Expr> expr, std::vector<U<Expr>> params) {
    return Expr(id, CallExpr(std::move(expr), std::move(params)));
  }
  static Expr makeLit(Id id, U<Lit> lit) { return Expr(id, LitExpr(std::move(lit))); }
  static Expr makeExpr(Id id, U<Expr> expr) { return Expr(id, ExprExpr(std::move(expr))); }
  static Expr makeLet(Id id, U<Pat> pattern, U<Expr> expr) {
    return Expr(id, LetExpr(std::move(pattern), std::move(expr)));
  }
  static Expr makePath(Id id, U<Path> path) { return Expr(id, PathExpr(std::move(path))); }
  static Expr makeIf(Id id, U<Expr> condition, U<Block> thenBlock) {
    return Expr(id,
        IfExpr(std::move(condition), std::move(thenBlock), std::nullopt));
  }

  static Expr makeIfElse(Id id, U<Expr> condition, U<Block> thenBlock,
                         U<Expr> elseExpr) {
    return Expr(id, IfExpr(std::move(condition), std::move(thenBlock),
                       std::move(elseExpr)));
  }

  static Expr makeLoop(Id id, U<Block> block) {
    return Expr(id, LoopExpr(std::move(block)));
  }

  static Expr makeWhile(Id id, U<Expr> condition, U<Block> block) {
    return Expr(id, WhileExpr(std::move(condition), std::move(block)));
  }
  static Expr makeBlock(Id id, U<Block> block) {
    return Expr(id, BlockExpr(std::move(block)));
  }
  static Expr makeBinary(Id id, BinOp op, U<Expr> first, U<Expr> second) {
    return Expr(id, BinaryExpr(std::move(op), std::move(first), std::move(second)));
  }

  static Expr makeAssign(Id id, U<Expr> first, U<Expr> second) {
    return Expr(id, AssignExpr(std::move(first), std::move(second)));
  }

  static Expr makeAssignOp(Id id, BinOp op, U<Expr> first, U<Expr> second) {
    return Expr(id, AssignOpExpr(std::move(op), std::move(first), std::move(second)));
  }
};
} // namespace MRC::AST
