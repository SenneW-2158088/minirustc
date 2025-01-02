#pragma once

#include <vector>

#include "ast/prelude.h"
#include "prelude.h"
#include "util/util.h"

namespace MRC::TST {

// Main HIR container that owns all nodes
struct Tst {

  std::vector<Expr> expressions;
  std::vector<Stmt> statements;
  std::vector<Block> blocks;
  std::vector<Type> types;
  std::vector<Local> locals;

  Index insert_expr(Expr expr) {
    expressions.push_back(std::move(expr));
    return expressions.size() - 1;
  }

  Index insert_stmt(Stmt stmt) {
    statements.push_back(std::move(stmt));
    return statements.size() - 1;
  }

  Index insert_block(Block block) {
    blocks.push_back(std::move(block));
    return blocks.size() - 1;
  }

  Index insert_type(Type type) {
    types.push_back(std::move(type));
    return types.size() - 1;
  }

  Index insert_local(Local local) {
    locals.push_back(std::move(local));
    return locals.size() - 1;
  }

  // Accessor methods
  const Expr &get_expr(Index idx) const { return expressions[idx]; }
  const Stmt &get_stmt(Index idx) const { return statements[idx]; }
  const Block &get_block(Index idx) const { return blocks[idx]; }
  const Type &get_type(Index idx) const { return types[idx]; }
  const Local &get_local(Index idx) const { return locals[idx]; }
};
} // namespace MRC::TST
