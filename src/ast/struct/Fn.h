#pragma once

#include "ast/prelude.h"
#include "ast/struct/Block.h"
#include <optional>

namespace MRC::AST {

struct Param {
  U<Type> type;
  U<Pat> pat;
  Param() = default;
  Param(U<Type> type, U<Pat> pat)
      : type(std::move(type)), pat(std::move(pat)) {}
};

struct Fn {
  using Params = std::vector<Param>;
  using Body = std::optional<U<Block>>;
  using ReturnType = std::optional<U<Type>>;

  Params params;
  Body body;
  ReturnType type;

  Fn() = default;
  Fn(Params params, Body body, ReturnType returnType = std::nullopt)
      : params(std::move(params)), body(std::move(body)),
        type(std::move(returnType)) {}
};
} // namespace MRC::AST
