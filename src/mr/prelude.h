#pragma once

#include <string>
#include <variant>

#include "ast/prelude.h"
#include "typechecking/Type.h"

namespace MRC::MR {
using Id = int;
using Symbol = std::string;

struct Pat;
struct Path;
struct Expr;
struct Stmt;
struct Block;
struct Body;
} // namespace MRC::MR

// Literal
namespace MRC::MR {

struct IntegerLit {};
struct BooleanLit {};
struct FloatLit {};
struct StrLit {};

struct Lit {
  using LiteralKind = std::variant<IntegerLit, BooleanLit, StrLit, FloatLit>;
  Symbol symbol;
  LiteralKind kind;
  TS::Type type;
};

struct IdentPat {
  AST::BindingMode binding;
  Symbol identifier;
  Opt<U<Pat>> pattern;

  IdentPat() = default;
  explicit IdentPat(AST::BindingMode binding, Symbol identifier, Opt<U<Pat>> pattern)
      : binding(std::move(binding)), identifier(std::move(identifier)),
        pattern(std::move(pattern)) {}
};

struct LitPat {
  Id expr;
};

struct Pat {
  using PatKind = std::variant<IdentPat, LitPat>;
  PatKind kind;
public:
  Pat() = default;
  Pat(PatKind kind) : kind(std::move(kind)) {}
};

struct Path {
  using PathSegments = std::vector<Symbol>;
  PathSegments segments;
};

struct Param {
  Id id;
  Pat pat;
  TS::Type type;
};

struct Fn {
  using Params = std::vector<Id>;
  using Body = std::optional<Id>;

  Params params;
  Body body;
  TS::Type type;
};

struct Body {
  Id id;
  std::vector<Param> params;
  Id expr;
};


struct Block {
  using Statements = std::vector<Id>;
  Statements statements{};
  Id id;
  TS::Type type;
};

struct DeclLocal {};

struct InitLocal {
  Id expr;
};

struct InitElseLocal {
  Id expr;
  Id block;
};

struct Local {
  using LocalKind = std::variant<DeclLocal, InitLocal, InitElseLocal>;
  Pat pat;
  LocalKind kind;
  TS::Type type;
  Id id;
};

/* Literal expression */
struct LitExpr {
  Lit lit;
};

/* Expression expression */
struct ExprExpr {
  Id expr;
};

/* Let expression */
struct LetExpr {
  Pat pattern;
  Id expr;
};

struct BlockExpr {
  Id block;
};

struct IfExpr {
  using ElseExpr = std::optional<Id>;
  Id expr;
  Id block;
  ElseExpr elseExpr;
};

struct LoopExpr {
  Id block;
};

struct WhileExpr {
  Id expr;
  Id block;
};

struct PathExpr {
  Path path;
};

struct CallExpr {
  Id expr;
  std::vector<Id> params;
};

struct BinaryExpr {
  AST::BinOp op;
  Id first;
  Id second;
};

struct AssignExpr {
  Id first;
  Id second;
};

struct AssignOpExpr {
  AST::BinOp op;
  Id first;
  Id second;
};

struct ContinueExpr {};

struct BreakExpr {
  Opt<Id> expr;
};

struct ReturnExpr {
  Opt<Id> expr;
};

struct UnitExpr{};

struct Expr {
  using ExprKind =
      std::variant<LitExpr, ExprExpr, LetExpr, BlockExpr, WhileExpr, IfExpr,
                   LoopExpr, CallExpr, PathExpr, BinaryExpr, AssignExpr,
                   AssignOpExpr, BreakExpr, ReturnExpr, ContinueExpr, UnitExpr>;
  ExprKind kind;
  Id id;
  TS::Type type;
};

struct ExprStmt {
    Id expr;
};

struct LetStmt {
    Pat pattern;
    Opt<Id> initializer;
    Opt<Id> else_block;
};

struct Stmt {
    using StmtKind = std::variant<
        ExprStmt,
        LetStmt
    >;
    Id id;
    StmtKind kind;
    Stmt(StmtKind kind) : kind(std::move(kind)) {}
};

} // namespace MRC::MR

// #include "mr/Mr.h"
