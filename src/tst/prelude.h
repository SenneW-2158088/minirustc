#pragma once

#include <cstdint>
#include <string>
#include <vector>

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

#include "tst/Expr.h"
#include "tst/Type.h"
#include "tst/Stmt.h"
#include "tst/Block.h"
#include "tst/Function.h"
#include "tst/Local.h"
