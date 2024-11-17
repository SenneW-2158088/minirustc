#pragma once

#include "ast/Lit.h"
#include "util/util.h"

#include <variant>

namespace MRC::AST {

struct Expr; // Forward declare

struct LiteralExpr {
  U<Lit> lit;
};

struct ExprExpr {
  U<Expr> lit;
};

struct Expr {
  std::variant<LiteralExpr, ExprExpr> kind{};

public:
  Expr() = default;
};

} // namespace MRC::AST
