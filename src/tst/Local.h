#pragma once

#include "tst/prelude.h"

namespace MRC::TST {

struct Local {
    Index pattern;
    Index type;
    Opt<Index> initializer;
    Opt<Index> else_block;

    Local() = default;
    Local(Index pattern, Index type,
          Opt<Index> initializer = std::nullopt,
          Opt<Index> else_block = std::nullopt)
        : pattern(pattern), type(type),
          initializer(initializer), else_block(else_block) {}
};

} // namespace MRC::TST
