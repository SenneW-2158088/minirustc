#pragma once

#include "ast/Body.h"
#include "ast/Expr.h"
#include "ast/struct/Block.h"
#include "ast/struct/Local.h"
#include "ast/struct/Pat.h"
#include "ast/visitor/Visitor.h"
#include "typechecking/Type.h"
#include "util/util.h"

#include <map>
#include <stdexcept>
#include <string>
#include <variant>

namespace MRC::TS {

class TypeError : public std::runtime_error {
public:
  explicit TypeError(const std::string &msg) : std::runtime_error(msg) {}
};

struct Scope {
    std::map<std::string, AST::Id> symbols;

    void insert(std::string symbol, AST::Id id, CheckType type) {
        auto symbol_id = symbols.find(symbol);

        if (symbol_id == symbols.end()) {
          std::cout << "Inserting new symbol: " << symbol << " with type " << type.to_string() << std::endl;
          symbols.emplace(symbol, id);
          return;
        }

        throw TypeError("Symbol " + symbol + " is already defined");
    }

    AST::Id resolve(std::string symbol) {
      auto si = symbols.find(symbol);
      if(si != symbols.end()) {
        return si->second;
      }

      throw TypeError("Symbol " + symbol + " is not defined");
    }
};

struct TypeContext {

  std::map<int, int> mapping;
  std::map<int, CheckType> ranks;
  int rank_counter = 0;


  void insert(AST::Id id, CheckType type) {
    int rank = rank_counter++;
    mapping.emplace(id, rank);
    ranks.emplace(rank, std::move(type));
    std::cout << "Inserted " << id << " as " << type.to_string() << " with rank " << rank << std::endl;
  }

  // Merge ranks
  void merge(AST::Id left, AST::Id right) {
    int left_rank = mapping[left];
    mapping[right] = left_rank;
  }

  void unionize(AST::Id left, AST::Id right) {
    auto *left_type = resolve(left);
    auto *right_type = resolve(right);

    std::cout << "unionize " << left << "->" << left_type->to_string() << " with " << right << "->" << right_type->to_string() << std::endl;

    // If types are equal merge ranks
    if(left_type->equals(*right_type)) {
      merge(left, right);
      return;
    }

    // Try to unify types
    if(left_type->unionize(*right_type)) {
      merge(left, right);
      return;
    }

    if(right_type->unionize(*left_type)) {
      merge(right, left);
      return;
    }

    // else throw an error
    throw TypeError("Types do not match: " + left_type->to_string() + " and " + right_type->to_string());
  }

  CheckType *resolve(AST::Id id) {
    try {
      int rank = mapping.at(id);
      auto it = ranks.find(rank);
      if (it == ranks.end()) {
        throw TypeError("Type not found for rank: " + std::to_string(rank) + " and id " + std::to_string(id));
      }
      return &it->second;
    } catch (const std::out_of_range &e) {
      throw TypeError("Failed to resolve type for node with id: " + std::to_string(id));
    }
  }


  void print_context() {
    std::cout << "Ranks: " << std::endl;
    for(auto const &[key, val] : ranks) {
      std::cout << key << " -> " << val.kind.index() << std::endl;
    }
    std::cout << "Mappings: " << std::endl;
    for(auto const &[key, val] : mapping) {
      auto type = resolve(key);
      std::cout << key << " -> " << val << " -> " << type->to_string() << std::endl;
    }
  }
};

class TypeChecker : public AST::Visitor {
private:
  void insert_symbol(std::string symbol, AST::Id id, CheckType type) {
    scopes.back().insert(symbol, id, std::move(type));
    context.insert(id, std::move(type));
  }

  AST::Id lookup_symbol(std::string symbol) {
    for(int i = scopes.size() - 1; i >= 0; --i) {
      try {
        return scopes[i].resolve(symbol);
      }
      catch( TypeError e) { continue; }
    }

    throw TypeError("Symbol " + symbol + " is not defined");
  }

  void push_scope() { scopes.push_back(Scope()); }
  void pop_scope() { scopes.pop_back(); }

public:
  TypeContext context;
  std::vector<Scope> scopes;
  TypeChecker() : context() { push_scope(); }

  void visit_body(AST::Body &body) override {
    push_scope();
    Visitor::visit_body(body);

    context.insert(body.id, CheckType::makeVar(Type::MakeUnset()));
    context.unionize(body.id, body.value->id);
    pop_scope();
  }

  void visit_item(AST::Item &item) override {
    push_scope();
    Visitor::visit_item(item);
    std::cout << "visit item called" << std::endl;

    std::visit(overloaded{[&](AST::FnItem &fn) {
     if (fn.fn->type.has_value()) {
       auto t = fn.fn->type.value()->to_type();
       context.insert(item.id, CheckType::makeConcrete(std::move(t)));
     } else {
       context.insert(item.id, CheckType::makeVar(Type::makeVoid()));
     }
   }},
   item.kind);
    pop_scope();
  }

  void visit_expr(AST::Expr &expr) override {
    Visitor::visit_expr(expr);

    context.insert(expr.id, std::move(expr.type));

    std::visit(
        overloaded{
            [&](AST::LitExpr &lit) {
                context.unionize(expr.id, lit.lit->id);
            },
            [&](AST::LetExpr &let) {
              if (std::holds_alternative<AST::IdentPat>(let.pattern->kind)) {
                auto &pat = std::get<AST::IdentPat>(let.pattern->kind);
                context.unionize(expr.id, let.expr->id);
              }
            },
            [&](AST::BlockExpr &block) { },
            [&](AST::WhileExpr &while_expr) {},
            [&](AST::LoopExpr &loop) { },
            [&](AST::IfExpr &if_expr) {
              context.insert(if_expr.expr->id, std::move(if_expr.expr->type));
              context.unionize(expr.id, if_expr.expr->id);
            },
            [&](AST::ExprExpr &expr_expr) {
              context.insert(expr_expr.expr->id, std::move(expr_expr.expr->type));
              context.unionize(expr.id, expr_expr.expr->id);
            },
            [&](AST::CallExpr &call) {
              context.insert(call.expr->id, std::move(call.expr->type));
              context.unionize(expr.id, call.expr->id);
            },
            [&](AST::PathExpr &path) {
              auto id = lookup_symbol(path.path->to_string());
              context.unionize(expr.id, id);
            },   
            [&](AST::BinaryExpr &bin) {
                context.insert(bin.first->id, std::move(bin.first->type));
                context.insert(bin.second->id, std::move(bin.second->type));

                context.unionize(bin.first->id, bin.second->id);
                context.unionize(expr.id, bin.first->id);
            }, 
            [&](AST::AssignExpr &assign) {
                context.insert(assign.first->id, std::move(assign.first->type));
                context.insert(assign.second->id, std::move(assign.second->type));

                context.unionize(assign.first->id, assign.second->id);
                context.unionize(expr.id, assign.first->id);
            }, 
            [&](AST::AssignOpExpr &assign_op) {
                context.insert(assign_op.first->id, std::move(assign_op.first->type));
                context.insert(assign_op.second->id, std::move(assign_op.second->type));

                context.unionize(assign_op.first->id, assign_op.second->id);
                context.unionize(expr.id, assign_op.first->id);
            },
          }, expr.kind);
  }

  void visit_stmt(AST::Stmt &stmt) override {
    Visitor::visit_stmt(stmt);
    std::visit(
        overloaded{
          [&](AST::LetStmt &let) {},
          [&](AST::ExprStmt &expr) {},
          [&](AST::ItemStmt &item) {},
          [&](AST::SemiStmt &item) {},
          [&](AST::EmptyStmt &item) {}
        }, stmt.kind);
  }

  void visit_lit(AST::Lit &lit) override {
    context.insert(lit.id, std::move(lit.check_type));
  }

  void visit_block(AST::Block &block) override {
    push_scope();
    Visitor::visit_block(block);
    pop_scope();
  }

  void visit_local(AST::Local &local) override {
    Visitor::visit_local(local);


    if (std::holds_alternative<AST::IdentPat>(local.pat->kind)) {
      auto &pat = std::get<AST::IdentPat>(local.pat->kind);


      if (local.type.has_value()) {
        local.check_type = CheckType::makeConcrete(local.type.value()->to_type());
      }

      std::visit(overloaded{
        [&](AST::InitLocal &init) {
          insert_symbol(pat.identifier.symbol, local.id, std::move(local.check_type));
          context.unionize(local.id, init.expr->id);
        },
        [&](AST::InitElseLocal &init) {
          insert_symbol(pat.identifier.symbol, local.id, std::move(local.check_type));
          context.unionize(local.id, init.expr->id);
        },
        [&](AST::DeclLocal &decl) {
          insert_symbol(pat.identifier.symbol, local.id, std::move(local.check_type));
        }},
     local.kind);
    }
  }
};
} // namespace MRC::TS
