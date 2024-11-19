#pragma once

#include <variant>

#include "Block.h"
#include "ast/Expr.h"
#include "ast/Type.h"
#include "ast/struct/Pat.h"
#include "util/util.h"

namespace MRC::AST {
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

  explicit Local(LocalKind kind) : kind(std::move(kind)) {}

  static Local makeDecl() { return Local(DeclLocal()); }
  static Local makeInit(U<Expr> expr) { return Local(InitLocal(std::move(expr))); }
  static Local makeInitElse(U<Expr> expr, U<Block> block) { return Local(InitElseLocal(std::move(expr), std::move(block))); }
};
} // namespace MRC::AST::STRUCT