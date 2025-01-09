#include "Visitor.h"
#include "ast/Expr.h"
#include "ast/Item.h"
#include "ast/Stmt.h"
#include "ast/struct/Fn.h"
#include "util/util.h"
#include <cstdio>
#include <unistd.h>

using namespace MRC::AST;

namespace MRC::AST {
void walk_stmt(Visitor *visitor, Stmt &stmt) {
  std::visit(
      overloaded{[&visitor](ExprStmt &val) { visitor->visit_expr(*val.expr); },
                 [&visitor](LetStmt &val) { visitor->visit_local(*val.local); },
                 [&visitor](ItemStmt &val) { visitor->visit_item(*val.item); },
                 [&visitor](SemiStmt &val) { visitor->visit_expr(*val.expr); },
                 [&visitor](EmptyStmt &val) { return; },
                 [&visitor](PrintStmt &val) { visitor->visit_expr(*val.expr); },
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
                 [&visitor](BlockExpr &val) { visitor->visit_block(*val.block); },
                 [&visitor](IfExpr &val) {
                     visitor->visit_expr(*val.expr);
                     visitor->visit_block(*val.block);
                     if(val.elseExpr.has_value()) {
                        visitor->visit_expr(*val.elseExpr.value());
                     }
                 },
                 [&visitor](CallExpr &val) {
                     visitor->visit_expr(*val.expr);
                     for (auto &param : val.params) {
                         visitor->visit_expr(*param);
                     }
                 },
                 [&visitor](PathExpr &val) { visitor->visit_path(*val.path); },
                 [&visitor](BreakExpr &val) {
                   if(val.expr.has_value()){
                    visitor->visit_expr(*val.expr.value());
                   }
                 },
                 [&visitor](ReturnExpr &val) {
                   if(val.expr.has_value()){
                    visitor->visit_expr(*val.expr.value());
                   }
                 },
                 [&visitor](ContinueExpr &val) {},
                 [&visitor](LoopExpr &val) { visitor->visit_block(*val.block); },
                 [&visitor](BinaryExpr &val) {
                     visitor->visit_expr(*val.first);
                    visitor->visit_expr(*val.second);
                 },
                 [&visitor](AssignExpr &val) {
                     visitor->visit_expr(*val.first);
                     visitor->visit_expr(*val.second);
                 },
                 [&visitor](AssignOpExpr &val) {
                     visitor->visit_expr(*val.first);
                     visitor->visit_expr(*val.second);
                 },
                 [&visitor](WhileExpr &val) {
                     visitor->visit_expr(*val.expr);
                     visitor->visit_block(*val.block);
                 }},
      expr.kind);
}

void walk_lit(Visitor *visitor, Lit &lit) {

  visitor->visit_symbol(lit.symbol);

  if(lit.suffix.has_value()) {
      visitor->visit_symbol(lit.suffix.value());
  }
}

void walk_ident(Visitor *visitor, Ident &ident) {
  // nothing to do
  visitor->visit_symbol(ident.symbol);
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
  visitor->visit_pat(*local.pat);
  std::visit(
      overloaded{[visitor](InitLocal &val) { visitor->visit_expr(*val.expr); },
                 [visitor](InitElseLocal &val) {
                   visitor->visit_expr(*val.expr);
                   visitor->visit_block(*val.block);
                 },
                 [](auto &val) { /* do nothing */ }},
      local.kind);
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
                 [visitor](LitPat &val) { visitor->visit_expr(*val.expr); },
                 [visitor](auto &val) { std::cout << "none" << std::endl; }},
      pat.kind);
}

void walk_path(Visitor *visitor, Path &path) {
  // Do nothing
  for(auto &segment : path.segments) {
      visitor->visit_symbol(segment);
  }
}

void walk_item(Visitor *visitor, Item &item) {
  std::visit(
      overloaded{
          [visitor](FnItem &val) {
            visitor->visit_fn(*val.fn);
          },
      },
      item.kind);

  walk_ident(visitor, item.ident);
}

void walk_fn(Visitor *visitor, Fn &fn) {
    // Parse params
    for(Param &param : fn.params)  {
      visitor->visit_param(param);
   }

   // parse body
   if(fn.body.has_value()) {
       visitor->visit_block(*fn.body.value());
   }

   // parse return type
   if(fn.type.has_value()) {
       visitor->visit_type(*fn.type.value());
   }
}

void walk_body(Visitor *visitor, Body &body) {

  for (auto &param : body.params) {
    visitor->visit_pat(*param.pat);
    visitor->visit_type(*param.type);
  }

  visitor->visit_expr(*body.value);
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
void Visitor::visit_item(Item &item) { walk_item(this, item); }
void Visitor::visit_symbol(Symbol &symbol) { return; }
void Visitor::visit_fn(Fn &fn) { walk_fn(this, fn); }
void Visitor::visit_body(Body &body) { walk_body(this, body); }
void Visitor::visit_param(Param &param) {
  walk_pat(this, *param.pat);
  walk_type(this, *param.type);
}
}
// namespace MRC::AST
