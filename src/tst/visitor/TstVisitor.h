#pragma once

#include "ast/Item.h"
#include "ast/Stmt.h"
#include "ast/struct/Block.h"
#include "ast/struct/Fn.h"
#include "ast/visitor/Visitor.h"
#include "tst/prelude.h"
#include "tst/tst.h"
#include "util/util.h"
#include <map>
#include <variant>

namespace MRC::TST {
class TstVisitor : public AST::Visitor {
private:
  Tst tst;
  std::map<std::string, Index> symbol_table;

public:
  // Lowering methods
  Index lower_fn(AST::Fn &fn) {
      Block block;

      for(const auto &param : fn.params) {
        tst.insert_local(TST::Local{
        });
      }

      if (fn.body) {
          for (auto& statement : fn.body.value().get()->statements) {
              auto stmt_idx = lower_statement(statement);
            )
          }
      }

      return tst.insert_block(block);
  };

  Index lower_block(AST::Block &block) {

  };

  Index lower_type() {

  };

  Index lower_statement(AST::Stmt &stmt) {
      return std::visit(overloaded{
         [&](auto &val) -> Index {
             return tst.insert_stmt(Stmt::);
         }
      }, stmt.kind);
  }

  void visit_item(AST::Item &item) override {
    std::visit(overloaded{[&](AST::FnItem &val) {
       auto block_idx = lower_fn(*val.fn.get());
       symbol_table[item.ident.symbol] = block_idx;
    }}, item.kind);
  }
  void visit_stmt(AST::Stmt &stmt) override {
    std::visit(overloaded{
                   [&](AST::ExprStmt &val) {},
                   [&](AST::LetStmt &val) {},
                   [&](AST::ItemStmt &val) {},
                   [&](AST::SemiStmt &val) {},
                   [&](AST::EmptyStmt &val) {},
               },
               stmt.kind);
  }
  void visit_expr(AST::Expr &expr) override {
    std::visit(
        overloaded{[&](AST::LitExpr &val) {}, [&](AST::ExprExpr &val) {},
                   [&](AST::LetExpr &val) {}, [&](AST::BlockExpr &val) {},
                   [&](AST::IfExpr &val) {}, [&](AST::CallExpr &val) {},
                   [&](AST::PathExpr &val) {}, [&](AST::LoopExpr &val) {},
                   [&](AST::WhileExpr &val) {}},
        expr.kind);
  }
};

struct TstBuilder {
  TST::Tst build(MRC::AST::Ast &ast) { TstVisitor visitor; }
};
} // namespace MRC::TST
