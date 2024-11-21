#pragma once

#include "ast/Expr.h"
#include "ast/Ident.h"
#include "ast/Lit.h"
#include "ast/Stmt.h"
#include "ast/Type.h"
#include "ast/struct/Block.h"
#include "ast/struct/Local.h"
#include "ast/visitor/Walker.h"

namespace MRC::AST {
struct Visitor {
public:
  // Root types
  virtual void visit_stmt(Stmt &stmt) { walk_stmt(this, stmt); }
  virtual void visit_expr(Expr &expr) { walk_expr(this, expr); }

  virtual void visit_lit(Lit &lit) { walk_lit(this, lit); }
  virtual void visit_ident(Ident &ident) { walk_ident(this, ident); }
  virtual void visit_type(Type &type) { walk_type(this, type); }

  virtual void visit_block(Block &block) { walk_block(this, block); }
  virtual void visit_local(Local &local) { walk_local(this, local); }
  virtual void visit_pat(Pat &pat) { walk_pat(this, pat); }
  virtual void visit_path(Path &path) { walk_path(this, path); }
};
} // namespace MRC::AST
