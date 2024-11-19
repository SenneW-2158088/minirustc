#pragma once

#include "ast/Expr.h"
#include "ast/struct/Local.h"
#include "util/util.h"

#include <variant>

namespace MRC::AST {

struct Stmt;

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

// To be supported
// Item(P<Item>),
// Semi(P<Expr>),

struct Stmt {
  using StmtKind = std::variant<ExprStmt, LetStmt, EmptyStmt>;
  StmtKind kind{};

public:
  Stmt() = default;
  explicit Stmt(StmtKind kind) : kind(std::move(kind)) {}

  static Stmt makeEmpty() { return Stmt(EmptyStmt()); }
  static Stmt makeExpr(U<Expr> expr) { return Stmt(ExprStmt(std::move(expr))); }
  static Stmt makeLet(U<Local> local) {
    return Stmt(LetStmt(std::move(local)));
  }
};
} // namespace MRC::AST
