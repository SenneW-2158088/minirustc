#include "interpreter/Interpreter.h"
#include "ast/Item.h"
#include "typechecking/TypeChecker.h"
#include <variant>

namespace MRC::INTERP {
// Flow
void Flow::entry() {}

void Flow::exit() {}

void Flow::next() {}

void Flow::stop() {}

Value Environment::evalute_item(AST::Item &item) {
  return Value();
}

Value Environment::evaluate_expr(AST::Expr &expr) {
  return Value();
}

Value Environment::evaluate_stmt(AST::Expr &expr) {
  return Value();
}

Value Environment::evaluate_local(AST::Local &expr) {
  return Value();
}

Value Environment::evaluate_block(AST::Block &expr) {
  return Value();
}


// Interpreter

Interpreter::Interpreter(P<TS::TypeContext> context) : type_context(context) {}
void Interpreter::visit_item(AST::Item &item) {
  std::visit(overloaded{[&](AST::FnItem &val) {

             }},
             item.kind);
}

void Interpreter::visit_stmt(AST::Stmt &stmt) {
  std::visit(overloaded{[&](auto &val) {

             }},
             stmt.kind);
}

void Interpreter::visit_expr(AST::Expr &expr) {
  std::visit(overloaded{[&](auto &val) {

             }},
             expr.kind);
}

void Interpreter::visit_local(AST::Local &local) {
  std::visit(overloaded{[&](auto &val) {

             }},
             local.kind);
}

AST::FnItem* resolve_function_node(std::string name, TS::TypeContext &context) {
  // int node_id = context.
  return nullptr;
}

void Interpreter::interp(std::string &entry) {
  auto *fn = resolve_function_node(entry, *this->type_context);
}


} // namespace MRC::INTERP
