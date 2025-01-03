#pragma once
#include "ast/struct/Local.h"
#include "ast/struct/Pat.h"
#include "ast/visitor/Visitor.h"
#include "typechecking/Type.h"
#include "util/util.h"
#include <map>
#include <stack>
#include <stdexcept>
#include <variant>

namespace MRC::TS {

class TypeError : public std::runtime_error {
public:
  explicit TypeError(const std::string &msg) : std::runtime_error(msg) {}
};

struct TypeScope {
  std::map<Symbol, CheckType> symbols;
  std::map<Symbol, CheckType> functions;

  // Insert a new symbol into the local typescope
  // throws an error if the symbol is already defined
  void insert(Symbol symbol, CheckType type) {
    if(symbols.contains(symbol)){
        throw TypeError("Symbol " + symbol +" already exists");
    }
    std::cout << "Inserted new symbol " << symbol << ":" << type.to_string() << std::endl;
    symbols.insert({symbol, std::move(type)});
  }

  // Tries to unionize a type with a symbol
  void unionize(Symbol symbol, CheckType type) {
      auto it = symbols.find(symbol);
      if(it == symbols.end()) {
          throw TypeError("Symbol " + symbol + " is not defined");
      }
      CheckType &stored = it->second;

      std::cout << "comparing stored " << symbol << "-> "<< stored.to_string() << " with " << type.to_string() << std::endl;
      if(!stored.unionize(type)) {
          throw TypeError("Types do not match: " + stored.to_string() + " and " + type.to_string());
      }

      std::cout << "set type of " << symbol << " to " << type.to_string() << std::endl;
  }
};

class TypeChecker : public AST::Visitor {
private:
  std::stack<TypeScope> scopes;

  void push_scope() {
      std::cout << "starting new scope" << std::endl;
      scopes.push(TypeScope{});
  }

  void pop_scope() {
    if (!scopes.empty()) {
      scopes.pop();
      std::cout << "popping scope" << std::endl;
    }
  }

public:
  TypeChecker() { push_scope(); }

  void visit_item(AST::Item &item) override {
    Visitor::visit_item(item);

    std::visit(overloaded{
        [&](AST::FnItem &fn) {
            if (fn.fn->type.has_value()) {
            auto t = fn.fn->type.value()->to_type();
            scopes.top().insert(item.ident.symbol,
                                CheckType::makeConcrete(std::move(t)));
            } else {
            scopes.top().insert(item.ident.symbol,
                                CheckType::makeVar(Type::makeVoid()));
            }
    }},
    item.kind);

  }

  void visit_expr(AST::Expr &expr) override {
    Visitor::visit_expr(expr);
    std::visit(overloaded{
        [&](AST::LitExpr &lit) {
            expr.type.unionize(lit.lit->check_type);
        },
        [&](AST::LetExpr &let) {
            if(std::holds_alternative<AST::IdentPat>(let.pattern->kind)) {
              auto &pat = std::get<AST::IdentPat>(let.pattern->kind);

              // scopes.top().insert(pat.identifier.symbol, CheckType::makeVar(Type::MakeUnset()));
              scopes.top().unionize(pat.identifier.symbol, std::move(let.expr->type));
            }
        },
        [&](AST::BlockExpr &block) {},
        [&](AST::WhileExpr &while_expr) {},
        [&](AST::LoopExpr &loop) {},
        [&](AST::IfExpr &if_expr) {},
        [&](AST::ExprExpr &if_expr) {},
        [&](AST::CallExpr &if_expr) {},
        [&](AST::PathExpr &path) {
        },
        [&](auto &replace_me) {}},
    expr.kind);
  }

  void visit_stmt(AST::Stmt &stmt) override {
      Visitor::visit_stmt(stmt);
      std::visit(overloaded{
        [&](AST::LetStmt &let) {
        },
        [&](AST::ExprStmt &expr) {
        },
                   [&](AST::ItemStmt &item) {}, [&](AST::SemiStmt &item) {},
                   [&](AST::EmptyStmt &item) {}},
        stmt.kind);
  }

  void visit_local(AST::Local &local) override {
      Visitor::visit_local(local);
      if(std::holds_alternative<AST::IdentPat>(local.pat->kind)) {
          auto &pat = std::get<AST::IdentPat>(local.pat->kind);

          if(local.type.has_value()) {
              local.check_type = CheckType::makeConcrete(local.type.value()->to_type());
          }

          std::visit(overloaded{
              [&](AST::InitLocal &init) {
                  scopes.top().insert(pat.identifier.symbol, std::move(local.check_type));
                  scopes.top().unionize(pat.identifier.symbol, std::move(init.expr->type));
                  local.check_type.unionize(init.expr->type);
              },
              [&](AST::InitElseLocal &init) {
                  scopes.top().insert(pat.identifier.symbol, std::move(local.check_type));
                  scopes.top().unionize(pat.identifier.symbol, std::move(init.expr->type));
                  local.check_type.unionize(init.expr->type);
              },
              [&](AST::DeclLocal &decl) {
                  scopes.top().insert(pat.identifier.symbol, std::move(local.check_type));
              }
          }, local.kind);
      }
  }
};
} // namespace MRC::TS
