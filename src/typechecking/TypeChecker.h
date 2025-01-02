#pragma once
#include "ast/visitor/Visitor.h"
#include "typechecking/Type.h"
#include <map>
#include <stack>
#include <stdexcept>

namespace MRC::TS {

class TypeError : public std::runtime_error {
public:
  explicit TypeError(const std::string &msg) : std::runtime_error(msg) {}
};

struct TypeScope {
  std::map<Symbol, AST::Type> symbols;

  // Insert a new symbol into the local typescope
  // throws an error if the symbol is already defined
  void insert(Symbol symbol, CheckType type) {}

  // Tries to unionize a type with a symbol
  // throws an error if this isn't possible
  void unionize(Symbol symbol, CheckType type) {}
};

class TypeChecker : public AST::Visitor {
private:
  std::stack<TypeScope> scopes;

  void push_scope() { scopes.push(TypeScope{}); }

  void pop_scope() {
    if (!scopes.empty()) {
      scopes.pop();
    }
  }

public:
  TypeChecker() { push_scope(); }

  void visit_item(AST::Item &item) override {
    push_scope();
    std::visit(overloaded{[&](AST::FnItem &fn) {
                 if (fn.fn->type.has_value()) {
                   auto t = fn.fn->type.value()->to_type();
                   scopes.top().insert(item.ident.symbol,
                                       CheckType::makeConcrete(std::move(t)));
                 } else {
                   scopes.top().insert(item.ident.symbol,
                                       CheckType::makeVar(Type::MakeUnset()));
                 }
               }},
               item.kind);

    Visitor::visit_item(item);

    pop_scope();
  }

  void visit_expr(AST::Expr &expr) override {
    Visitor::visit_expr(expr);
    std::visit(overloaded{[&](AST::LitExpr &lit) {
    },
                          [&](AST::LetExpr &let) {},
                          [&](AST::BlockExpr &block) {},
                          [&](AST::WhileExpr &while_expr) {},
                          [&](AST::LoopExpr &loop) {},
                          [&](AST::IfExpr &if_expr) {},
                          [&](AST::ExprExpr &if_expr) {},
                          [&](AST::CallExpr &if_expr) {},
                          [&](AST::PathExpr &if_expr) {}},
               expr.kind);
  }

  void visit_stmt(AST::Stmt &stmt) override {
    std::visit(
        overloaded{[&](AST::LetStmt &let) {}, [&](AST::ExprStmt &expr) {},
                   [&](AST::ItemStmt &item) {}, [&](AST::SemiStmt &item) {},
                   [&](AST::EmptyStmt &item) {}},
        stmt.kind);
  }
};
} // namespace MRC::TS
