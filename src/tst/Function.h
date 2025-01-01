#pragma once

#include "tst/prelude.h"
#include <vector>

namespace MRC::TST {

struct Param {
    Index pattern;
    Index type;

    Param() = default;
    Param(Index pattern, Index type)
        : pattern(pattern), type(type) {}
};

struct Function {
    std::vector<Param> params;
    Index return_type;
    Opt<Index> body;
    std::vector<Index> locals;

    Function() = default;
    Function(std::vector<Param> params,
            Index return_type,
            Opt<Index> body = std::nullopt,
            std::vector<Index> locals = {})
        : params(std::move(params)),
          return_type(return_type),
          body(body),
          locals(std::move(locals)) {}
};

} // namespace MRC::TST
