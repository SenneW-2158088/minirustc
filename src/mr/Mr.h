#pragma once

#include <exception>
#include <map>
#include <stdexcept>
#include <string>

#include "ast/Ast.h"
#include "ast/Item.h"
#include "ast/Lit.h"
#include "ast/Stmt.h"
#include "ast/struct/Block.h"
#include "ast/struct/Fn.h"
#include "ast/struct/Local.h"
#include "typechecking/Type.h"
#include "typechecking/TypeChecker.h"

#include "mr/prelude.h"
#include "util/util.h"

namespace MRC::MR {

struct SymbolTable {

  struct Symbol {
    Id id;
    TS::Type type;
    P<SymbolTable> subscope;
    enum class Kind { Func, Var, Type, Block} kind;

    Symbol(Id id, TS::Type type, P<SymbolTable> subscope, Kind k = Kind::Var)
      : id(id), type(type), kind(k), subscope(subscope) {}
  };

  SymbolTable *parent;
  std::map<std::string, P<Symbol>> symbols;

  SymbolTable() : parent(nullptr) {}

  explicit SymbolTable(SymbolTable* parent_scope) : parent(parent_scope) {}
  Opt<P<Symbol>> insert(std::string &name, Symbol &symbol);
  Opt<P<Symbol>> lookup(std::string &name);
  Opt<P<SymbolTable>> subscope();

  void print() const;
  void print_internal(int indent) const;
};

struct Mr {
public:
  P<SymbolTable> tree;
  std::vector<P<Expr>> expressions;
  std::vector<P<Stmt>> statements;
  std::vector<P<Block>> blocks;
  std::vector<P<Local>> locals;
  std::vector<P<Param>> params;
  std::vector<P<Fn>> functions;

public:
  Id insert_expr(Expr expr) {
    auto id = expressions.size();
    expr.id = id;
    expressions.push_back(std::make_shared<Expr>(std::move(expr)));
    return id;
  }

  Id insert_stmt(Stmt stmt) {
    auto id = statements.size();
    stmt.id = id;
    statements.push_back(std::make_shared<Stmt>(std::move(stmt)));
    return id;
  }

  Id insert_block(Block block) {
    auto id = blocks.size();
    block.id = id;
    blocks.push_back(std::make_shared<Block>(std::move(block)));
    return id;
  }

  Id insert_local(Local local) {
    auto id = locals.size();
    local.id = id;
    locals.push_back(std::make_shared<Local>(std::move(local)));
    return id;
  }

  Id insert_param(Param param) {
    auto id = params.size();
    param.id = id;
    params.push_back(std::make_shared<Param>(std::move(param)));
    return id;
  }

  Id insert_fn(Fn fn) {
    auto id = functions.size();
    fn.id = id;
    functions.push_back(std::make_shared<Fn>(std::move(fn)));
    return id;
  }

  P<Expr> get_expr(Id id) { return expressions[id]; }
  P<Stmt> get_stmt(Id id) { return statements[id]; }
  P<Block> get_block(Id id) { return blocks[id]; }
  P<Local> get_local(Id id) { return locals[id]; }
  P<Param> get_param(Id id) { return params[id]; }
  P<Fn> get_fn(Id id) { return functions[id]; }

  void dump() const {
    std::cout << "Expressions" << std::endl;
    for (auto &expr : expressions) {
      std::cout << expr->id << std::endl;
    }

    std::cout << "Statements" << std::endl;
    for (auto &stmt : statements) {
      std::cout << stmt->id << std::endl;
    }

    std::cout << "Blocks" << std::endl;
    for (auto &block : blocks) {
      std::cout << block->id << std::endl;
    }

    std::cout << "Locals" << std::endl;
    for (auto &local : locals) {
      std::cout << local->id << std::endl;
    }

    for (auto &param : params) {
      std::cout << param->id << std::endl;
    }

    std::cout << "Functions" << std::endl;
    for (auto &fn : functions) {
      std::cout << fn->id << std::endl;
    }
  }
};

struct MrBuilder {
private:
  P<TS::TypeContext> type_context;
  Mr mr;
  std::vector<P<SymbolTable>> scopes;

public:
  MrBuilder(P<TS::TypeContext> context) : type_context(context), mr{} {
    scopes.push_back(std::make_shared<SymbolTable>(nullptr));
  }

  P<SymbolTable> pop_scope() {
    auto stored = scopes.back();
    scopes.pop_back();
    return stored;
  }

  P<SymbolTable::Symbol> insert_symbol(std::string &name, SymbolTable::Symbol symbol) {
    auto result = scopes.back()->insert(name, symbol);
    if(result.has_value()) return result.value();
    throw std::runtime_error("symbol already exists");
  }

  P<SymbolTable> push_scope() {
    auto result = scopes.back()->subscope();
    if(result.has_value()) {
      scopes.push_back(result.value());
      return result.value();
    }
    throw std::runtime_error("failed to create subscope");
  }


  Id make_block(AST::Block &block);
  Id make_expr(AST::Expr &expr);
  Id make_fn(AST::Fn &expr);
  Id make_param(AST::Param &params);
  Id make_stmt(AST::Stmt &stmt);
  Id make_local(AST::Local &local);
  Path make_path(AST::Path &path);
  Stmt lower_local(AST::Local &local);
  Stmt lower_item(AST::Item &item);

  Lit make_lit(AST::Lit &lit);
  Pat make_pat(AST::Pat &lit);

  Mr build(AST::Ast &ast);
};
} // namespace MRC::MR
