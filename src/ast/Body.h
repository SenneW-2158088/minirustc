#pragma once

#include "ast/prelude.h"
#include "ast/struct/Fn.h"

namespace MRC::AST {
  struct Body {
    Id id;
    std::vector<Param> params;
    U<Expr> value;
  public:
    Body() = default;

    Body(Id id, std::vector<Param> params, U<Expr> value)
      : id(id)
      , params(std::move(params))
      , value(std::move(value)) {}

  };
}
