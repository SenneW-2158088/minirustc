#pragma once

#include "tst/prelude.h"
#include "util/util.h"
#include <variant>

namespace MRC::TST {

struct ExprStmt {
    Index expr;
};

struct LetStmt {
    Index pattern;
    Opt<Index> initializer;
    Opt<Index> else_block;
};

struct Stmt {
    using StmtKind = std::variant<
        ExprStmt,
        LetStmt
    >;

    StmtKind kind;

    // Helper constructors
    static Stmt makeExpr(Index expr) {
        return Stmt{ExprStmt{expr}};
    }

    static Stmt makeLet(
        Index pattern,
        Opt<Index> initializer = std::nullopt,
        Opt<Index> else_block = std::nullopt
    ) {
        return Stmt{LetStmt{
            pattern,
            initializer,
            else_block
        }};
    }

    Stmt(StmtKind kind) : kind(std::move(kind)) {}
};
} // namespace MRC::HIR
