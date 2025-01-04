#pragma once

#include <vector>

#include "ast/prelude.h"

namespace MRC::AST {

using StmtPtr = U<Stmt>;
using ExprPtr = U<Expr>;

typedef int Index;

struct Ast {
  std::vector<Body> bodies{};
  std::vector<Item> items{};
  std::vector<Stmt> statements{};
  std::vector<Expr> expressions{};
  Id id_index = 0;

public:
  Ast() = default;

  Index insert(Item item) {
    items.push_back(std::move(item));
    return items.size() - 1;
  }

  Index insert(Stmt stmt) {
    statements.push_back(std::move(stmt));
    return statements.size() - 1;
  }

  Index insert(Expr expr) {
    expressions.push_back(std::move(expr));
    return expressions.size() - 1;
  }

  Index insert(Body body) {
    bodies.push_back(std::move(body));
    return bodies.size() - 1;
  }

  Id getId() {
    return id_index++;
  }
};

} // namespace MRC::AST
