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
  Id id;

public:
  Stmt() = default;
  Stmt(Id id, StmtKind kind) : id(id), kind(std::move(kind)) {}

  static Stmt makeEmpty(Id id) { return Stmt(id, EmptyStmt()); }
  static Stmt makeExpr(Id id, U<Expr> expr) { return Stmt(id, ExprStmt(std::move(expr))); }
  static Stmt makeLet(Id id, U<Local> local) {
    return Stmt(id, LetStmt(std::move(local)));
  }
  static Stmt makeItem(Id id, U<Item> item) { return Stmt(id, ItemStmt(std::move(item))); }

  static Stmt makeSemi(Id id, U<Expr> expr) { return Stmt(id, SemiStmt(std::move(expr))); }
};
} // namespace MRC::AST
