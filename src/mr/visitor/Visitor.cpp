#include "Visitor.h"
#include "ast/Expr.h"
#include "ast/Item.h"
#include "ast/struct/Fn.h"
#include "mr/prelude.h"
#include "util/util.h"
#include <cstdio>
#include <unistd.h>


namespace MRC::MR{
void walk_stmt(Visitor *visitor, Stmt &stmt) {
  std::visit(
      overloaded{
      [&visitor](ExprStmt &val) {
        visitor->visit_expr(val.expr);
      },
      [&visitor](PrintStmt &val) {
        visitor->visit_expr(val.expr);
      },
      [&visitor](LetStmt &val) {
        visitor->visit_pat(val.pattern);
        if(val.initializer.has_value()) {
          visitor->visit_expr(val.initializer.value());
        }
        if(val.else_block.has_value()) {
          visitor->visit_expr(val.else_block.value());
        }
      }
     }, stmt.kind);
}

void walk_expr(Visitor *visitor, Expr &expr) {
  std::visit(
      overloaded{[&visitor](LitExpr &val) { visitor->visit_lit(val.lit); },
                 [&visitor](ExprExpr &val) { visitor->visit_expr(val.expr); },
                 [&visitor](LetExpr &val) {
                   visitor->visit_expr(val.expr);
                   visitor->visit_pat(val.pattern);
                 },
                 [&visitor](BlockExpr &val) { visitor->visit_block(val.block); },
                 [&visitor](IfExpr &val) {
                     visitor->visit_expr(val.expr);
                     visitor->visit_block(val.block);
                     if(val.elseExpr.has_value()) {
                        visitor->visit_expr(val.elseExpr.value());
                     }
                 },
                 [&visitor](CallExpr &val) {
                     visitor->visit_expr(val.expr);
                     for (auto &param : val.params) {
                         visitor->visit_expr(param);
                     }
                 },
                 [&visitor](PathExpr &val) { visitor->visit_path(val.path); },
                 [&visitor](BreakExpr &val) {
                   if(val.expr.has_value()){
                    visitor->visit_expr(val.expr.value());
                   }
                 },
                 [&visitor](ReturnExpr &val) {
                   if(val.expr.has_value()){
                    visitor->visit_expr(val.expr.value());
                   }
                 },
                 [&visitor](ContinueExpr &val) {},
                 [&visitor](LoopExpr &val) { visitor->visit_block(val.block); },
                 [&visitor](BinaryExpr &val) {
                     visitor->visit_expr(val.first);
                    visitor->visit_expr(val.second);
                 },
                 [&visitor](AssignExpr &val) {
                     visitor->visit_expr(val.first);
                     visitor->visit_expr(val.second);
                 },
                 [&visitor](AssignOpExpr &val) {
                     visitor->visit_expr(val.first);
                     visitor->visit_expr(val.second);
                 },
                 [&visitor](UnitExpr &val) { },
                 [&visitor](WhileExpr &val) {
                     visitor->visit_expr(val.expr);
                     visitor->visit_block(val.block);
                 }},
      expr.kind);
}

void walk_lit(Visitor *visitor, Lit &lit) {
  visitor->visit_symbol(lit.symbol);
}

void walk_block(Visitor *visitor, Block &block) {
  for (auto &stmt : block.statements) {
    visitor->visit_stmt(stmt);
  }
}

void walk_local(Visitor *visitor, Local &local) {
  visitor->visit_pat(local.pat);
  std::visit(
      overloaded{
      [visitor](InitLocal &val) { visitor->visit_expr(val.expr); },
      [visitor](InitElseLocal &val) {
         visitor->visit_expr(val.expr);
         visitor->visit_block(val.block);
       },
       [](auto &val) { /* do nothing */ }},
      local.kind);
}

void walk_pat(Visitor *visitor, Pat &pat) {
  std::visit(
      overloaded{[visitor](IdentPat &val) {
                   visitor->visit_symbol(val.identifier);
                   if (val.pattern.has_value()) {
                     visitor->visit_pat(*val.pattern.value());
                   }
                 },
                 [visitor](LitPat &val) { visitor->visit_expr(val.expr); },
                 [visitor](auto &val) { std::cout << "none" << std::endl; }},
      pat.kind);
}

void walk_path(Visitor *visitor, Path &path) {
  // Do nothing
  for(auto &segment : path.segments) {
      visitor->visit_symbol(segment);
  }
}

void walk_fn(Visitor *visitor, Fn &fn) {
    // Parse params
    for(auto &param : fn.params)  {
      visitor->visit_param(param);
   }

   // parse body
   if(fn.body.has_value()) {
       visitor->visit_block(fn.body.value());
   }
}

void walk_body(Visitor *visitor, Body &body) {

  for (auto &param : body.params) {
    visitor->visit_pat(param.pat);
  }

  visitor->visit_expr(body.expr);
}

void Visitor::visit_stmt(Id id) {
  walk_stmt(this, *mr->get_stmt(id));
}

void Visitor::visit_expr(Id id) {
  walk_expr(this, *mr->get_expr(id));
}

void Visitor::visit_lit(Lit& lit) {
  walk_lit(this, lit);
}

void Visitor::visit_block(Id id) {
  walk_block(this, *mr->get_block(id));
}

void Visitor::visit_local(Id id) {
  walk_local(this, *mr->get_local(id));
}

void Visitor::visit_pat(Pat& pat) {
  walk_pat(this, pat);
}

void Visitor::visit_path(Path& path) {
  walk_path(this, path);
}

void Visitor::visit_symbol(std::string& symbol) {
  // Base implementation - can be overridden
}

void Visitor::visit_fn(Id id) {
  walk_fn(this, *mr->get_fn(id));
}

void Visitor::visit_param(Id id) { }
}
// namespace MRC::AST
