#pragma once

#include <iostream>
#include <string>
#include <variant>

#include "Token.h"
#include "ast/Expr.h"
#include "ast/Type.h"
#include "ast/visitor/Visitor.h"
#include "typechecking/TypeChecker.h"
#include "util/util.h"

namespace MRC::AST {
struct PrintVisitor : Visitor {
private:
  int indent_level = 0;
  const std::string indent_str = "  "; // Two spaces per level

  void print_indent() {
    for (int i = 0; i < indent_level; i++) {
      std::cout << indent_str;
    }
  }

  P<TS::TypeContext> context;

  struct ScopeGuard {
    int &level;
    ScopeGuard(int &l) : level(l) { level++; }
    ~ScopeGuard() { level--; }
  };

public:

  PrintVisitor(P<TS::TypeContext> context) : context(context) {}

  void visit_stmt(Stmt &stmt) override {
    print_indent();
    std::cout << "Stmt {\n";

    print_indent();
    std::cout << "id: " << stmt.id << std::endl;
    {
      ScopeGuard guard(indent_level);
      Visitor::visit_stmt(stmt);
    }
    print_indent();
    std::cout << "}\n";
  }

  void visit_expr(Expr &expr) override {
    print_indent();
    std::cout << "Expr {\n";

    print_indent();
    std::cout << "Type:" << context->resolve(expr.id)->to_string() << std::endl;

    print_indent();
    std::cout << "id: " << expr.id << std::endl;

    if(std::holds_alternative<BinaryExpr>(expr.kind)){
        auto &bin_expr = std::get<BinaryExpr>(expr.kind);
        print_indent();
        std::cout << "operation:" << bin_expr.op.to_string() << std::endl;
    }
    if(std::holds_alternative<AssignExpr>(expr.kind)){
        auto &bin_expr = std::get<BinaryExpr>(expr.kind);
        print_indent();
        std::cout << "operation:" << bin_expr.op.to_string() << std::endl;
    }
    if(std::holds_alternative<AssignOpExpr>(expr.kind)){
        auto &bin_expr = std::get<BinaryExpr>(expr.kind);
        print_indent();
        std::cout << "operation:" << bin_expr.op.to_string() << std::endl;
    }

    {
      ScopeGuard guard(indent_level);
      Visitor::visit_expr(expr);
    }
    print_indent();
    std::cout << "}\n";
  }

  void visit_lit(Lit &lit) override {
    print_indent();
    std::cout << "Lit {\n";
    print_indent();
    std::cout << "Type:" << context->resolve(lit.id)->to_string() << std::endl;

    print_indent();
    std::cout << "id: " << lit.id << std::endl;

    {
      ScopeGuard guard(indent_level);

      Visitor::visit_lit(lit);

      std::visit(overloaded {
        [](auto &val){}
      }, lit.kind);

    }
    print_indent();
    std::cout << "}\n";
  }


  void visit_ident(Ident &ident) override {
    print_indent();
    std::cout << "Ident {\n";
    {
      ScopeGuard guard(indent_level);
      Visitor::visit_ident(ident);
    }
    print_indent();
    std::cout << "}\n";
  }

  void visit_type(Type &type) override {
    print_indent();
    std::cout << "Type {\n";
    {
      ScopeGuard guard(indent_level);
      Visitor::visit_type(type);
    }
    print_indent();
    std::cout << "}\n";
  }

  void visit_block(Block &block) override {
    print_indent();
    std::cout << "Block {\n";
    {
      ScopeGuard guard(indent_level);
      Visitor::visit_block(block);
    }
    print_indent();
    std::cout << "}\n";
  }

  void visit_local(Local &local) override {
    print_indent();
    std::cout << "Local {\n";

    print_indent();
    std::cout << "Type:" << context->resolve(local.id)->to_string() << std::endl;

    print_indent();
    std::cout << "id: " << local.id << std::endl;

    if(local.type) {
        print_indent();
        std::cout << "Type:" << local.check_type.to_string() << std::endl;
    }
    {
      ScopeGuard guard(indent_level);
      Visitor::visit_local(local);
    }
    print_indent();
    std::cout << "}\n";
  }

  void visit_pat(Pat &pat) override {
    print_indent();
    std::cout << "Pat {\n";
    {
      ScopeGuard guard(indent_level);
      Visitor::visit_pat(pat);
    }
    print_indent();
    std::cout << "}\n";
  }

  void visit_path(Path &path) override {
    print_indent();
    std::cout << "Path {\n";
    {
      ScopeGuard guard(indent_level);
      Visitor::visit_path(path);
    }
    print_indent();
    std::cout << "}\n";
  }

  void visit_item(Item &item) override {
      print_indent();
      std::cout << "Item {\n";

      print_indent();
      std::cout << "type: " << context->resolve(item.id)->to_string() << std::endl;

      print_indent();
      std::cout << "id: " << item.id << std::endl;
      {
        ScopeGuard guard(indent_level);
        Visitor::visit_item(item);
      }
      print_indent();
      std::cout << "}\n";
  }

  void visit_symbol(Symbol &symbol) override {
      print_indent();
      std::cout << "Symbol { " << symbol << " }" << std::endl;
  }

  void visit_fn(Fn &fn) override {
      print_indent();
      std::cout << "Fn {\n";

      {
        ScopeGuard guard(indent_level);
        Visitor::visit_fn(fn);
      }
      print_indent();
      std::cout << "}\n";
  }

};
} // namespace MRC::AST
