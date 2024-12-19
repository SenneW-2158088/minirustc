#pragma once
#include "ast/prelude.h"

namespace MRC::AST {
// Forward declare the visitor class first
struct Visitor;

// Forward declare all walk functions
void walk_stmt(Visitor *visitor, Stmt &stmt);
void walk_expr(Visitor *visitor, Expr &expr);
void walk_lit(Visitor *visitor, Lit &lit);
void walk_ident(Visitor *visitor, Ident &ident);
void walk_type(Visitor *visitor, Type &type);
void walk_block(Visitor *visitor, Block &block);
void walk_local(Visitor *visitor, Local &local);
void walk_pat(Visitor *visitor, Pat &pat);
void walk_path(Visitor *visitor, Path &path);
void walk_item(Visitor *visitor, Item &path);

// Then define the visitor struct
struct Visitor {
public:
  // Root types
  virtual void visit_stmt(Stmt &stmt);
  virtual void visit_expr(Expr &expr);
  virtual void visit_lit(Lit &lit);
  virtual void visit_ident(Ident &ident);
  virtual void visit_type(Type &type);
  virtual void visit_block(Block &block);
  virtual void visit_local(Local &local);
  virtual void visit_pat(Pat &pat);
  virtual void visit_path(Path &path);
  virtual void visit_item(Item &path);
};
} // namespace MRC::AST
