#pragma once

#include "ast/Expr.h"
#include "ast/Ident.h"
#include "ast/Stmt.h"
#include "ast/Type.h"
#include "ast/struct/Local.h"
#include "ast/struct/Pat.h"
#include "ast/visitor/Visitor.h"
#include "util/util.h"
#include <variant>

namespace MRC::AST {
struct Visitor;

void walk_stmt(Visitor *visitor, Stmt &stmt) {
  std::visit(
      overloaded{[&visitor](ExprStmt &val) { visitor->visit_expr(*val.expr); },
                 [&visitor](LetStmt &val) { visitor->visit_local(*val.local); },
                 [&visitor](EmptyStmt &val) { return; }},
      stmt.kind);
}

void walk_expr(Visitor *visitor, Expr &expr) {
  std::visit(
      overloaded{[&visitor](LitExpr &val) { visitor->visit_lit(*val.lit); },
                 [&visitor](ExprExpr &val) { visitor->visit_expr(*val.expr); },
                 [&visitor](LetExpr &val) {
                   visitor->visit_expr(*val.expr);
                   visitor->visit_pat(*val.pattern);
                 }},
      expr.kind);
} // namespace MRC::AST

void walk_lit(Visitor *visitor, Lit &lit) {
  std::visit([](auto &val) {}, lit.kind);
} // namespace MRC::AST

void walk_ident(Visitor *visitor, Ident &ident) {
  // nothing to do
} // namespace MRC::AST

void walk_type(Visitor *visitor, Type &type) {
  std::visit(overloaded{
                 [visitor](PathType &val) { visitor->visit_path(*val.path); },
                 [](auto &val) {},
             },
             type.kind);
} // namespace MRC::AST

void walk_block(Visitor *visitor, Block &block) {
  for (auto &stmt : block.statements) {
    visitor->visit_stmt(stmt);
  }
} // namespace MRC::AST

void walk_local(Visitor *visitor, Local &local) {
  std::visit(
      overloaded{[visitor](InitLocal &val) { visitor->visit_expr(*val.expr); },
                 [visitor](InitElseLocal &val) {
                   visitor->visit_expr(*val.expr);
                   visitor->visit_block(*val.block);
                 },
                 [](auto &val) { // do nothing}
                 }},
      local.kind);
} // namespace MRC::AST

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
} // namespace MRC::AST
