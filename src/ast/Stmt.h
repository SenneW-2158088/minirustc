#pragma once

#include "ast/prelude.h"

#include "util/util.h"

namespace MRC::AST {
struct Pat;
struct Local;
struct Expr;

struct ExprStmt {
  U<Expr> expr;

public:
  ExprStmt() = default;
  explicit ExprStmt(U<Expr> expr) : expr(std::move(expr)) {}
};

struct LetStmt {
  U<Local> local;

public:
  LetStmt() = default;
  explicit LetStmt(U<Local> local) : local(std::move(local)) {}
};

struct EmptyStmt {};

struct ItemStmt {
  U<Item> item;
  ItemStmt() = default;
  explicit ItemStmt(U<Item> item) : item(std::move(item)) {}
};

struct SemiStmt {
  U<Expr> expr;

public:
  SemiStmt() = default;
  explicit SemiStmt(U<Expr> expr) : expr(std::move(expr)) {}
};

struct Stmt {
  using StmtKind =
      std::variant<ExprStmt, LetStmt, EmptyStmt, ItemStmt, SemiStmt>;
  StmtKind kind{};

public:
  Stmt() = default;
  Stmt(StmtKind kind) : kind(std::move(kind)) {}

  static Stmt makeEmpty() { return Stmt(EmptyStmt()); }
  static Stmt makeExpr(U<Expr> expr) { return Stmt(ExprStmt(std::move(expr))); }
  static Stmt makeLet(U<Local> local) {
    return Stmt(LetStmt(std::move(local)));
  }
  static Stmt makeItem(U<Item> item) { return Stmt(ItemStmt(std::move(item))); }

  static Stmt makeSemi(U<Expr> expr) { return Stmt(SemiStmt(std::move(expr))); }
};
} // namespace MRC::AST
