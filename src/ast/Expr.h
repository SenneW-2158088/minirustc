#pragma once

#include "ast/Lit.h"

#include <variant>

namespace MRC::AST {

struct LiteralExpr {
  Lit lit;
};

struct Expr {
  std::variant<LiteralExpr> kind;
};

} // namespace MRC::AST
