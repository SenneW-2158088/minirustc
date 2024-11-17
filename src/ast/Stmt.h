#pragma once

#include "ast/Expr.h"

#include <variant>

namespace MRC::AST {

    struct ExprStmt {
        Expr expr;
    };

    struct Stmt {
        std::variant<ExprStmt> kind;
    };
}
