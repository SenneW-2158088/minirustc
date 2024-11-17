#pragma once

#include <vector>

namespace MRC::AST {

class Expr {};
class Stmt {};

class Ast {
    std::vector<Stmt> statements;
    std::vector<Expr> expressions;

    public:
    void insert(std::unique_ptr<Stmt> stmt);
};


}  // namespace MRC::AST
