#pragma once

#include <variant>
#include <vector>

#include "prelude.h"
#include "util/util.h"

namespace MRC::TST {

struct LiteralExpr {
    Index literal;
    Index type;
};

struct BlockExpr {
    Index block;
    Index type;
};

struct LocalExpr {
    Index local;
    Index type;
};

struct IfExpr {
    Index condition;
    Index then_block;
    Opt<Index> else_expr;
    Index type;
};

struct WhileExpr {
    Index condition;
    Index block;
    Index type;
};

struct LoopExpr {
    Index block;
    Index type;
};

struct VarRefExpr {
    std::string name;
    Index type;
};

struct Expr {
    using ExprKind = std::variant<
        LiteralExpr,
        BlockExpr,
        LocalExpr,
        IfExpr,
        WhileExpr,
        LoopExpr,
        VarRefExpr
    >;

    ExprKind kind;
    Index type;     // Index into types

    Expr(ExprKind kind, Index type)
        : kind(std::move(kind)), type(type) {}
};

} // namespace MRC::HIR
