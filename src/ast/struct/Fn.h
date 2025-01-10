#pragma once

#include "ast/prelude.h"
#include "typechecking/Type.h"
#include <optional>

namespace MRC::AST {

struct Param {
  U<Type> type;
  U<Pat> pat;
  Id id;
  Param() = default;
  Param(Id id, U<Type> type, U<Pat> pat)
      : id(id), type(std::move(type)), pat(std::move(pat)) {}
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

  TS::Type to_type() const {
    std::vector<U<TS::CheckType>> param_types;
    param_types.reserve(params.size());

    for (const auto &param : params) {
      if (param.type) {
        param_types.push_back(std::make_unique<TS::CheckType>(TS::CheckType::makeConcrete(param.type->to_type())));
      }
    }

    U<TS::CheckType> return_type;
    if(type.has_value()){
      return_type = MU<TS::CheckType>(TS::CheckType::makeConcrete(type.value()->to_type()));
    }
    else {
      return_type = MU<TS::CheckType>(TS::CheckType::makeConcrete(TS::Type::makeVoid()));
    }

    return TS::Type::makeFunction(std::move(param_types), std::move(return_type));
  }
};
} // namespace MRC::AST
