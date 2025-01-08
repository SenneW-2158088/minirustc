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
#include "typechecking/Type.h"
#include "typechecking/TypeChecker.h"

#include "mr/prelude.h"
#include "util/util.h"

namespace MRC::MR {

class SymbolTable {
private:
    struct Symbol;
    struct Scope {
        std::map<std::string, P<SymbolTable::Symbol>> symbols;
        P<Scope> parent;
        
        Scope(P<Scope> parent = nullptr) : parent(parent) {}
    };
    
    struct Symbol {
        enum class Kind { Function, Variable };
        Kind kind;
        Id id;  // Either local_id or block_id depending on kind
        TS::Type type;
    };
    
    P<Scope> current_scope;

public:
    SymbolTable() : current_scope(std::make_shared<Scope>()) {}
    
    void enter_scope() {
        current_scope = std::make_shared<Scope>(current_scope);
    }
    
    void exit_scope() {
        if (current_scope->parent) {
            current_scope = current_scope->parent;
        }
    }
    
    void insert(std::string name, TS::Type type, Symbol::Kind kind, Id id) {
        current_scope->symbols.emplace(name, std::make_shared<Symbol>(Symbol{kind, id, type}));
    }
    
    Opt<P<Symbol>> lookup(const std::string& name) {
        P<Scope> scope = current_scope;
        while (scope) {
            auto it = scope->symbols.find(name);
            if (it != scope->symbols.end()) {
                return it->second;
            }
            scope = scope->parent;
        }
        return std::nullopt;
    }
};

struct Mr {
public:
  SymbolTable tree;
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
    for(auto & expr : expressions) {
        std::cout << expr->id << std::endl;
    }

    std::cout << "Statements" << std::endl;
    for(auto & stmt : statements) {
        std::cout << stmt->id << std::endl;
    }

    std::cout << "Blocks" << std::endl;
    for(auto & block : blocks) {
        std::cout << block->id << std::endl;
    }

    std::cout << "Locals" << std::endl;
    for(auto & local : locals) {
        std::cout << local->id << std::endl;
    }

    std::cout << "Params" << std::endl;
    for(auto & param : params) {
        std::cout << param->id << std::endl;
    }

    std::cout << "Functions" << std::endl;
    for(auto & fn : functions) {
        std::cout << fn->id << std::endl;
    }
}
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
