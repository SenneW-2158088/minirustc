#pragma once

#include <vector>

#include "prelude.h"
#include "util/util.h"

namespace MRC::TST {

// Main HIR container that owns all nodes
struct Tst {
  std::vector<Expr> expressions;
  std::vector<Stmt> statements;
public:
    Tst() = default;
};
} // namespace MRC::TST
