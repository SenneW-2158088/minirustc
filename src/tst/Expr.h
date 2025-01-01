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
        LoopExpr,
        VarRefExpr
    >;

    ExprKind kind;
    Index type;

    Expr(ExprKind kind, Index type)
        : kind(std::move(kind)), type(type) {}

    static Expr makeLit(Index literal, Index type) {
        return Expr(LiteralExpr{literal, type}, type);
    }

    static Expr makeBlock(Index block, Index type) {
        return Expr(BlockExpr{block, type}, type);
    }

    static Expr makeLocal(Index local, Index type) {
        return Expr(LocalExpr{local, type}, type);
    }

    static Expr makeIf(Index condition, Index then_block,
                      Opt<Index> else_expr, Index type) {
        return Expr(IfExpr{condition, then_block, else_expr, type}, type);
    }

    static Expr makeLoop(Index block, Index type) {
        return Expr(LoopExpr{block, type}, type);
    }

    static Expr makeVarRef(std::string name, Index type) {
        return Expr(VarRefExpr{name, type}, type);
    }
};

} // namespace MRC::TST
