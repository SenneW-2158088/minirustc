#pragma once

#include <map>
#include <string>

#include "ast/Ast.h"
#include "ast/Item.h"
#include "ast/Lit.h"
#include "ast/Stmt.h"
#include "ast/struct/Block.h"
#include "ast/struct/Fn.h"
#include "ast/struct/Local.h"
#include "ast/visitor/Visitor.h"
#include "typechecking/Type.h"
#include "typechecking/TypeChecker.h"

#include "mr/prelude.h"
#include "util/util.h"

namespace MRC::MR {

struct SymbolTree;

struct SymbolNode {
  std::string symbol;
  Id id;
  P<SymbolTree> child_tree;
};

struct SymbolTree {
  std::map<std::string, SymbolNode> symbols;
};

struct Mr {
  std::vector<P<Expr>> expressions;
  std::vector<P<Stmt>> statements;
  std::vector<P<Block>> blocks;
  std::vector<P<Local>> locals;
  std::vector<P<Param>> params;
  std::vector<P<Fn>> functions;

public:
  Id insert_expr(Expr expr) {
    auto id = expressions.size();
    expressions.push_back(std::make_shared<Expr>(std::move(expr)));
    return id;
  }

  Id insert_stmt(Stmt stmt) {
    auto id = statements.size();
    statements.push_back(std::make_shared<Stmt>(std::move(stmt)));
    return id;
  }

  Id insert_block(Block block) {
    auto id = blocks.size();
    blocks.push_back(std::make_shared<Block>(std::move(block)));
    return id;
  }

  Id insert_local(Local local) {
    auto id = locals.size();
    locals.push_back(std::make_shared<Local>(std::move(local)));
    return id;
  }

  Id insert_param(Param param) {
    auto id = params.size();
    params.push_back(std::make_shared<Param>(std::move(param)));
    return id;
  }

  Id insert_fn(Fn fn) {
    auto id = functions.size();
    functions.push_back(std::make_shared<Fn>(std::move(fn)));
    return id;
  }

  P<Expr> get_expr(Id id) { return expressions[id]; }
  P<Stmt> get_stmt(Id id) { return statements[id]; }
  P<Block> get_block(Id id) { return blocks[id]; }
  P<Local> get_local(Id id) { return locals[id]; }
  P<Param> get_param(Id id) { return params[id]; }
  P<Fn> get_fn(Id id) { return functions[id]; }
};

struct MrBuilder {
private:
  P<TS::TypeContext> type_context;
  Mr mr;

public:
  MrBuilder(P<TS::TypeContext> context) : type_context(context) , mr{} {}
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
