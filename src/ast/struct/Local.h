#pragma once

#include "ast/prelude.h"

namespace MRC::AST {
struct Stmt;
struct Block;

struct DeclLocal {};

struct InitLocal {
  U<Expr> expr;
};

struct InitElseLocal {
  U<Expr> expr;
  U<Block> block;
};

struct Local {
  using LocalKind = std::variant<DeclLocal, InitLocal, InitElseLocal>;
  U<Pat> pat;
  LocalKind kind;
  Opt<U<Type>> type;

public:
  Local() = default;

  explicit Local(U<Pat> pat, Opt<U<Type>> type, LocalKind kind) : kind(std::move(kind)), pat(std::move(pat)), type(std::move(type)) {}

  static Local makeDecl(U<Pat> pat, Opt<U<Type>> type) {
      return Local(std::move(pat), std::move(type), DeclLocal());
  }
  static Local makeInit(U<Pat> pat, Opt<U<Type>> type, U<Expr> expr) {
    return Local(std::move(pat), std::move(type), InitLocal(std::move(expr)));
  }
  static Local makeInitElse(U<Pat> pat, Opt<U<Type>> type, U<Expr> expr, U<Block> block) {
    return Local(std::move(pat), std::move(type), InitElseLocal(std::move(expr), std::move(block)));
  }
};
} // namespace MRC::AST
