#pragma once

#include <iostream>
#include <string>
#include "mr/Mr.h"
#include "mr/prelude.h"
#include "mr/visitor/Visitor.h"

namespace MRC::MR {
struct PrintVisitor : Visitor {
private:
  int indent_level = 0;
  const std::string indent_str = "  "; // Two spaces per level

  void print_indent() {
    for (int i = 0; i < indent_level; i++) {
      std::cout << indent_str;
    }
  }

  struct ScopeGuard {
    int& level;
    ScopeGuard(int& l) : level(l) { level++; }
    ~ScopeGuard() { level--; }
  };

public:
  explicit PrintVisitor(Mr* mr) : Visitor(mr) {}

  void print() {
    visit_fn(0);
  }

  void visit_stmt(Id id) override {
    print_indent();
    std::cout << "Stmt[" << id << "] {\n";
        
    {
      ScopeGuard guard(indent_level);
      Visitor::visit_stmt(id);
    }
    print_indent();
    std::cout << "}\n";
  }

  void visit_expr(Id id) override {
    print_indent();
    auto expr = mr->get_expr(id);
    std::cout << "Expr[" << id <<  " : " << expr->type.to_string() <<  "] {\n";
    {
      ScopeGuard guard(indent_level);
      Visitor::visit_expr(id);
    }
    print_indent();
    std::cout << "}\n";
  }

  void visit_block(Id id) override {
    auto block = mr->get_block(id);
    std::cout << "Block[" << id << " : " << block->type.to_string() << "] {\n";
    {
      ScopeGuard guard(indent_level);
      Visitor::visit_block(id);
    }
    print_indent();
    std::cout << "}\n";
  }

    void visit_local(Id id) override {
    print_indent();
    auto local = mr->get_local(id);
    std::cout << "Local[" << id << " : " << local->type.to_string() << "] {\n";
    {
      ScopeGuard guard(indent_level);
      Visitor::visit_local(id);
    }
    print_indent();
    std::cout << "}\n";
  }

  void visit_param(Id id) override {
    print_indent();
    auto param = mr->get_param(id);
    std::cout << "Param[" << id << " : " << param->type.to_string() << "] {\n";
    {
      ScopeGuard guard(indent_level);
      Visitor::visit_param(id);
    }
    print_indent();
    std::cout << "}\n";
  }

  void visit_fn(Id id) override {
    print_indent();
    auto fn = mr->get_fn(id);
    std::cout << "Fn[" << id << " -> " << fn->type.to_string() << "] {\n";
    {
      ScopeGuard guard(indent_level);
      Visitor::visit_fn(id);
    }
    print_indent();
    std::cout << "}\n";
  }

  void visit_path(Path& path) override {
    print_indent();
    std::cout << "Path {\n";
    {
      ScopeGuard guard(indent_level);
      Visitor::visit_path(path);
    }
    print_indent();
    std::cout << "}\n";
  }

  void visit_lit( Lit& lit) override {
    print_indent();
    std::cout << "Lit[" << lit.symbol;
    std::cout << "] {\n";
    {
      ScopeGuard guard(indent_level);
      Visitor::visit_lit(lit);
    }
    print_indent();
    std::cout << "}\n";
  }

  void visit_pat(Pat& pat) override {
    print_indent();
    std::cout << "Pat {\n";
    {
      ScopeGuard guard(indent_level);
      Visitor::visit_pat(pat);
    }
    print_indent();
    std::cout << "}\n";
  }

  void visit_symbol(std::string& symbol) override {
    print_indent();
    std::cout << "Symbol { " << symbol << " }\n";
  }
};
} // namespace MRC::MR
