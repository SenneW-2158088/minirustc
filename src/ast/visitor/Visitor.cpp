#include "Visitor.h"

using namespace MRC::AST;

namespace MRC::AST {
void walk_stmt(Visitor *visitor, Stmt &stmt) {
  std::visit(
      overloaded{[&visitor](ExprStmt &val) { visitor->visit_expr(*val.expr); },
                 [&visitor](LetStmt &val) { visitor->visit_local(*val.local); },
                 [&visitor](EmptyStmt &val) { return; },
                 [&visitor](auto &val) { /* TODO */
                                         return;
                 }},
      stmt.kind);
}

void walk_expr(Visitor *visitor, Expr &expr) {
  std::visit(
      overloaded{[&visitor](LitExpr &val) { visitor->visit_lit(*val.lit); },
                 [&visitor](ExprExpr &val) { visitor->visit_expr(*val.expr); },
                 [&visitor](LetExpr &val) {
                   visitor->visit_expr(*val.expr);
                   visitor->visit_pat(*val.pattern);
                 },
                 [&visitor](auto &val) { /* TODO */ }},
      expr.kind);
}

void walk_lit(Visitor *visitor, Lit &lit) {
  std::visit([](auto &val) {}, lit.kind);
}

void walk_ident(Visitor *visitor, Ident &ident) {
  // nothing to do
}

void walk_type(Visitor *visitor, Type &type) {
  std::visit(overloaded{
                 [visitor](PathType &val) { visitor->visit_path(*val.path); },
                 [](auto &val) {},
             },
             type.kind);
}

void walk_block(Visitor *visitor, Block &block) {
  for (auto &stmt : block.statements) {
    visitor->visit_stmt(stmt);
  }
}

void walk_local(Visitor *visitor, Local &local) {
  std::visit(
      overloaded{[visitor](InitLocal &val) { visitor->visit_expr(*val.expr); },
                 [visitor](InitElseLocal &val) {
                   visitor->visit_expr(*val.expr);
                   visitor->visit_block(*val.block);
                 },
                 [](auto &val) { /* do nothing */ }},
      local.kind);
  visitor->visit_pat(*local.pat);
  if (local.type.has_value()) {
    visitor->visit_type(*local.type.value());
  }
}

void walk_pat(Visitor *visitor, Pat &pat) {
  std::visit(
      overloaded{[visitor](IdentPat &val) {
                   visitor->visit_ident(val.identifier);
                   if (val.pattern.has_value()) {
                     visitor->visit_pat(*val.pattern.value());
                   }
                 },
                 [visitor](LitPat &val) { visitor->visit_expr(*val.expr); }},
      pat.kind);
}

void walk_path(Visitor *visitor, Path &path) {
  // Do nothing
}

void Visitor::visit_stmt(Stmt &stmt) { walk_stmt(this, stmt); }
void Visitor::visit_expr(Expr &expr) { walk_expr(this, expr); }
void Visitor::visit_lit(Lit &lit) { walk_lit(this, lit); }
void Visitor::visit_ident(Ident &ident) { walk_ident(this, ident); }
void Visitor::visit_type(Type &type) { walk_type(this, type); }
void Visitor::visit_block(Block &block) { walk_block(this, block); }
void Visitor::visit_local(Local &local) { walk_local(this, local); }
void Visitor::visit_pat(Pat &pat) { walk_pat(this, pat); }
void Visitor::visit_path(Path &path) { walk_path(this, path); }
} // namespace MRC::AST
