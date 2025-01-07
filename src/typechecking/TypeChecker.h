#pragma once

#include "ast/Body.h"
#include "ast/Expr.h"
#include "ast/struct/Block.h"
#include "ast/struct/Fn.h"
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
      // std::cout << "Inserting new symbol: " << symbol << " with type "
      //           << type.to_string() << std::endl;
      symbols.emplace(symbol, id);
      return;
    }

    throw TypeError("Symbol " + symbol + " is already defined");
  }

  AST::Id resolve(std::string symbol) {
    auto si = symbols.find(symbol);
    if (si != symbols.end()) {
      return si->second;
    }

    throw TypeError("Symbol " + symbol + " is not defined");
  }

  void print_scope() {
    std::cout << "symbol table" << std::endl;
    for (const auto &[key, val] : symbols) {
      std::cout << key << " -> " << val << std::endl;
    }
  }
};

struct TypeContext {

  std::map<int, int> mapping;
  std::map<int, CheckType> ranks;
  int rank_counter = 0;

  void insert(AST::Id id, CheckType type) {
    if (mapping.find(id) != mapping.end()) {
      throw TypeError("Node with id " + std::to_string(id) +
                      " is already inserted");
    }
    int rank = rank_counter++;
    mapping.emplace(id, rank);
    ranks.emplace(rank, type);
    // std::cout << "Inserted " << id << " as " << type.to_string()
    //           << " with rank " << rank << std::endl;
  }

  // Merge ranks
  void merge(AST::Id left, AST::Id right) {
    int left_rank = mapping[left];
    int right_rank = mapping[right];
    mapping[right] = left_rank;

    for(auto &[id, rank] : mapping) {
      if (rank == right_rank) rank = left_rank;
    }

    // std::cout << right << " points to rank " << left_rank << " -> "
    //           << resolve(left)->to_string() << std::endl;
  }

  void unionize(AST::Id left, AST::Id right) {
    auto *left_type = resolve(left);
    auto *right_type = resolve(right);

    // std::cout << "unionize " << left << "->" << left_type->to_string()
    //           << " with " << right << "->" << right_type->to_string()
    //           << std::endl;

    // If types are equal merge ranks
    if (left_type->equals(*right_type)) {
      // std::cout << "set " << right << ":" << right_type->to_string() << " to " << left << ":" << left_type->to_string() << std::endl;
      merge(left, right);
      return;
    }

    // Try to unify types
    if (left_type->unionize(*right_type)) {
      merge(left, right);
      // std::cout << "set " << right << ":" << right_type->to_string() << " to " << left << ":" << left_type->to_string() << std::endl;
      return;
    }

    if (right_type->unionize(*left_type)) {
      merge(right, left);
      // std::cout << "set " << left << ":" << left_type->to_string() << " to " << right << ":" << right_type ->to_string() << std::endl;
      return;
    }

    // else throw an error
    throw TypeError("Types do not match: " + left_type->to_string() + " and " +
                    right_type->to_string());
  }

  CheckType *resolve(AST::Id id) {
    try {
      int rank = mapping.at(id);
      auto it = ranks.find(rank);
      if (it == ranks.end()) {
        throw TypeError("Type not found for rank: " + std::to_string(rank) +
                        " and id " + std::to_string(id));
      }
      return &it->second;
    } catch (const std::out_of_range &e) {
      throw TypeError("Failed to resolve type for node with id: " +
                      std::to_string(id));
    }
  }

  void print_context() {
    // std::cout << "Ranks: " << std::endl;
    // for (auto const &[key, val] : ranks) {
    //   std::cout << key << " -> " << val.kind.index() << std::endl;
    // }
    std::cout << "Mappings: " << std::endl;
    for (auto const &[key, val] : mapping) {
      auto type = resolve(key);
      std::cout << key << " -> " << val << " -> " << type->to_string()
                << std::endl;
    }
  }
};

class TypeChecker : public AST::Visitor {
private:
  void insert_symbol(std::string symbol, AST::Id id, CheckType type) {
    scopes.back().insert(symbol, id, type);
    context.insert(id, type);
  }

  std::optional<AST::Id> find_symbol_in_scope(const std::string &symbol,
                                              const Scope &scope) {
    auto it = scope.symbols.find(symbol);
    if (it != scope.symbols.end()) {
      return it->second;
    }
    return std::nullopt;
  }

  AST::Id lookup_symbol(const std::string &symbol) {
    std::cout << "lookup up symbol " << symbol << std::endl;
    for (auto it = scopes.rbegin(); it != scopes.rend(); ++it) {
      if (auto id = find_symbol_in_scope(symbol, *it)) {
        return *id;
      }
    }

    throw TypeError("Symbol '" + symbol + "' is not defined in current scope");
  }

  void push_scope() {
    scopes.push_back(Scope());
  }

  void pop_scope() {
    scopes.back().print_scope();
    scopes.pop_back();
  }

public:
  TypeContext context;
  std::vector<Scope> scopes;
  int scope_index = 0;
  TypeChecker() : context() { push_scope(); }
  ~TypeChecker() { pop_scope(); }

  void visit_item(AST::Item &item) override {
    push_scope();

    Visitor::visit_item(item);

    std::visit(overloaded{[&](AST::FnItem &fn) {
                 insert_symbol(item.ident.symbol, item.id,
                               CheckType::makeConcrete(fn.fn->to_type()));
               }},
               item.kind);
    push_scope();
  }

  void visit_param(AST::Param &param) override {
    if (std::holds_alternative<AST::IdentPat>(param.pat->kind)) {
      auto &ident = std::get<AST::IdentPat>(param.pat->kind);
      insert_symbol(ident.identifier.symbol, param.id,
                    CheckType::makeConcrete(param.type->to_type()));
    }
  }

  void visit_expr(AST::Expr &expr) override {
    Visitor::visit_expr(expr);

    context.insert(expr.id, expr.type);

    std::visit(
        overloaded{
            [&](AST::LitExpr &lit) { context.unionize(expr.id, lit.lit->id); },
            [&](AST::LetExpr &let) {
              std::visit(
                  overloaded{[&](AST::IdentPat &val) {
                               auto id = lookup_symbol(val.identifier.symbol);
                               context.unionize(let.expr->id, id);
                             },
                             [&](AST::LitPat &val) {
                               context.unionize(let.expr->id, val.expr->id);
                             }},
                  let.pattern->kind);
              context.unionize(expr.id, let.expr->id);
            },
            [&](AST::BlockExpr &block) {
              auto b_expr = block.block->expr();
              if (b_expr.has_value()) {
                context.unionize(expr.id, b_expr.value()->id);
              }
            },
            [&](AST::WhileExpr &while_expr) {
              context.unionize(expr.id, while_expr.expr->id);

              auto b_expr = while_expr.block->expr();
              if (b_expr.has_value()) {
                context.unionize(expr.id, b_expr.value()->id);
              }
            },
            [&](AST::LoopExpr &loop) {
              auto b_expr = loop.block->expr();
              if (b_expr.has_value()) {
                context.unionize(expr.id, b_expr.value()->id);
              }
            },
            [&](AST::IfExpr &if_expr) {
              context.unionize(expr.id, if_expr.expr->id);
            },
            [&](AST::ExprExpr &expr_expr) {
              context.unionize(expr.id, expr_expr.expr->id);
            },
            [&](AST::CallExpr &call) {

              auto t = FunctionType();
              for (auto &param : call.params) {
                // param->id
              }

              context.unionize(expr.id, call.expr->id);
            },
            [&](AST::PathExpr &path) {
              auto id = lookup_symbol(path.path->to_string());
              context.unionize(expr.id, id);
            },
            [&](AST::ReturnExpr &val) {
              if (val.expr.has_value()) {
                context.unionize(expr.id, val.expr.value()->id);
              }
            },
            [&](AST::ContinueExpr &val) {},
            [&](AST::BreakExpr &val) {
              if (val.expr.has_value()) {
                context.unionize(expr.id, val.expr.value()->id);
              }
            },
            [&](AST::BinaryExpr &bin) {
              context.unionize(bin.first->id, bin.second->id);
              context.unionize(expr.id, bin.first->id);
            },
            [&](AST::AssignExpr &assign) {
              context.unionize(assign.first->id, assign.second->id);
              context.unionize(expr.id, assign.first->id);
            },
            [&](AST::AssignOpExpr &assign_op) {
              context.unionize(assign_op.first->id, assign_op.second->id);
              context.unionize(expr.id, assign_op.first->id);
            },
        },
        expr.kind);
  }

  void visit_stmt(AST::Stmt &stmt) override {
    Visitor::visit_stmt(stmt);
    std::visit(
        overloaded{[&](AST::LetStmt &let) {}, [&](AST::ExprStmt &expr) {},
                   [&](AST::ItemStmt &item) {}, [&](AST::SemiStmt &item) {},
                   [&](AST::EmptyStmt &item) {}},
        stmt.kind);
  }

  void visit_lit(AST::Lit &lit) override {
    context.insert(lit.id, lit.check_type);
  }

  void visit_block(AST::Block &block) override {
    push_scope();
    Visitor::visit_block(block);
    pop_scope();
  }

  void visit_local(AST::Local &local) override {
    Visitor::visit_local(local);

    // if (local.type.has_value()) {
    //   context.insert(local.id,
    //                  CheckType::makeConcrete(local.type.value()->to_type()));
    // } else {
    //   context.insert(local.id, CheckType::makeVar(Type::makeUnset()));
    // }

    std::visit(
        overloaded{[&](AST::IdentPat &pat) {
                     std::visit(
                         overloaded{[&](AST::InitLocal &init) {
                              insert_symbol(pat.identifier.symbol, local.id, local.check_type);
                              context.unionize(local.id, init.expr->id);
                          },
                      [&](AST::InitElseLocal &init) {
                          insert_symbol(pat.identifier.symbol, local.id, local.check_type);
                          context.unionize(local.id, init.expr->id);
                        },
                        [&](AST::DeclLocal &decl) {
                          insert_symbol(pat.identifier.symbol, local.id, local.check_type);
                        }},
                        local.kind);
                   },
                   [&](AST::LitPat &val) {
                     std::visit(
                         overloaded{[&](AST::InitLocal &init) {
                              if (local.type.has_value()) {
                                context.insert(local.id,
                                               CheckType::makeConcrete(local.type.value()->to_type()));
                              } else {
                                context.insert(local.id, CheckType::makeVar(Type::makeUnset()));
                              }
                              context.unionize(init.expr->id, val.expr->id);
                              context.unionize(local.id, init.expr->id);
                          },
                      [&](AST::InitElseLocal &init) {
                              if (local.type.has_value()) {
                                context.insert(local.id,
                                               CheckType::makeConcrete(local.type.value()->to_type()));
                              } else {
                                context.insert(local.id, CheckType::makeVar(Type::makeUnset()));
                              }
                            context.unionize(init.expr->id, val.expr->id);
                            context.unionize(local.id, init.expr->id);
                        },
                        [&](AST::DeclLocal &decl) {
                              if (local.type.has_value()) {
                                context.insert(local.id,
                                               CheckType::makeConcrete(local.type.value()->to_type()));
                              } else {
                                context.insert(local.id, CheckType::makeVar(Type::makeUnset()));
                              }
                        }},
                        local.kind);
                   }},
        local.pat->kind);
    }
};
} // namespace MRC::TS
