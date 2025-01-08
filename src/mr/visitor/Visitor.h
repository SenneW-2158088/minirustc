#pragma once

#include "mr/prelude.h"
#include "mr/Mr.h"

namespace MRC::MR{
// Forward declare the visitor class first
struct Visitor;

// Forward declare all walk functions
void walk_stmt(Visitor *visitor, Stmt &stmt);
void walk_expr(Visitor *visitor, Expr &expr);
void walk_lit(Visitor *visitor, Lit &lit);
void walk_block(Visitor *visitor, Block &block);
void walk_local(Visitor *visitor, Local &local);
void walk_pat(Visitor *visitor, Pat &pat);
void walk_path(Visitor *visitor, Path &path);
void walk_body(Visitor *visitor, Body &path);

// Then define the visitor struct
struct Visitor {
private:
  Mr mr;
public:
  // Root types
  virtual void visit_stmt(Id &stmt);
  virtual void visit_expr(Id &expr);
  virtual void visit_lit(Lit &lit);
  virtual void visit_block(Id &block);
  virtual void visit_local(Id &local);
  virtual void visit_pat(Pat &pat);
  virtual void visit_path(Path &path);
  virtual void visit_symbol(Symbol &symbol);
  virtual void visit_fn(Id &fn);
  virtual void visit_body(Id &fn);
  virtual void visit_param(Id& param);
};
} // namespace MRC::AST
