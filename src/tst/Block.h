#pragma once

#include "tst/prelude.h"
#include <vector>

namespace MRC::TST {

struct Block {
  std::vector<Index> statements;
  Opt<Index> expr;
  Index type;

  Block() = default;
  Block(std::vector<Index> statements, Opt<Index> expr, Index type)
      : statements(std::move(statements)), expr(expr), type(type) {}
};

} // namespace MRC::TST
