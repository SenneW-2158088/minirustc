#pragma once

#include <cstdint>

namespace MRC::TST {
using Index = int32_t;
struct Expr;
struct Stmt;
struct Block;
struct Local;
struct Item;
struct Function;
struct Type;
struct Literal;
}  // namespace MRC::HIR

#include "tst/Type.h"
#include "tst/Expr.h"
// #include "hir/Stmt.h"
// #include "hir/Block.h"
// #include "hir/Function.h"
// #include "hir/Local.h"
// #include "hir/Item.h"
// #include "hir/Literal.h"
