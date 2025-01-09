#include "interpreter/Interpreter.h"
#include "mr/prelude.h"
#include "typechecking/TypeChecker.h"
#include <memory>
#include <unistd.h>
#include <variant>

namespace MRC::INTERP {
// Flow
void Flow::entry() {}

void Flow::exit() {}

void Flow::next() {}

void Flow::stop() {}

void Environment::print(int depth) const {
  std::string indent(depth * 2, ' ');

  // Print variables in current scope
  std::cout << indent << "Variables:\n";
  for (const auto &[name, var] : variables) {
    std::cout << indent << "  " << name << ": ";
    if (var->is_initialized()) {
      std::cout << var->value.value().to_string();
    } else {
      std::cout << "uninitialized";
    }
    std::cout << (var->is_mutable ? " (mut)" : "") << "\n";
  }

  // Recursively print child environments
  for (const auto &child : children) {
    std::cout << indent << "Child Environment:\n";
    child->print(depth + 1);
  }
}

void Environment::declare_variable(const std::string &name, bool is_mutable) {
  if (variables.find(name) != variables.end()) {
    throw RuntimeError("Variable '" + name +
                       "' already declared in this scope");
  }
  variables[name] = std::make_shared<Variable>();
  variables[name]->is_mutable = is_mutable;
}

void Environment::set_variable(const std::string &name, const Value &value) {
  auto var = lookup_variable_mut(name);
  if (!var) {
    throw RuntimeError("Undefined variable '" + name + "'");
  }

  if (!var->is_mutable && var->is_initialized()) {
    throw ImmutableError(name);
  }

  var->value = value;
}

Value Environment::get_variable(const std::string &name) {
  auto var = lookup_variable(name);
  if (!var || !var->is_initialized()) {
    throw RuntimeError("Use of uninitialized variable '" + name + "'");
  }
  return var->value.value();
}

bool Environment::has_variable(const std::string &name) {
  return lookup_variable(name) != nullptr;
}

Variable *Environment::lookup_variable(const std::string &name) {
  auto it = variables.find(name);
  if (it != variables.end()) {
    return it->second.get();
  }

  if (parent) {
    return parent->lookup_variable(name);
  }

  return nullptr;
}

Variable *Environment::lookup_variable_mut(const std::string &name) {
  auto it = variables.find(name);
  if (it != variables.end()) {
    return it->second.get();
  }

  if (parent) {
    return parent->lookup_variable_mut(name);
  }

  return nullptr;
}

Value Environment::evaluate_local(MR::Id &local) { return Value(); }

Value Environment::evaluate_fn(MR::Id fn) {
  auto node = mr.get_fn(fn);
  if (!node) {
    std::cout << "function node not found" << std::endl;
    throw std::runtime_error("Function node not found");
  }

  if (node->body.has_value()) {
    return evaluate_block(node->body.value());
  }
  return Value{}; // Return empty value for functions without body
}

Value Environment::evaluate_expr(MR::Id &expr) {
  auto node = mr.get_expr(expr);
  if (!node) {
    throw std::runtime_error("Expression node not found");
  }

  return std::visit(
      overloaded{[&](MR::LitExpr &val) -> Value { return Value(val.lit); },
                 [&](MR::BlockExpr &val) -> Value {
                   return evaluate_block(val.block);
                 },
                 [&](MR::CallExpr &val) -> Value {
                   auto fn_val = evaluate_expr(val.expr);
                   std::vector<Value> args;
                   for (auto &param : val.params) {
                     args.push_back(evaluate_expr(param));
                   }
                   // TODO: Call function with args
                   return Value{};
                 },
                 [&](MR::PathExpr &val) -> Value {
                   auto var = get_variable(val.path.to_string());
                   return var;
                 },
                 [&](MR::IfExpr &val) -> Value {

                   auto cond = evaluate_expr(val.expr);
                   if(cond.is_bool()) {
                     auto c = cond.as<bool>();
                     if(c) {
                        return evaluate_block(val.block);
                     }
                     else {
                       if(val.elseExpr.has_value()){
                          return evaluate_expr(val.elseExpr.value());
                       }
                     }
                     return Value();
                   }
                   throw RuntimeError("Invalid assignment target");
                 },
                 [&](MR::BinaryExpr &val) -> Value {
                   auto first_val = evaluate_expr(val.first);
                   auto second_val = evaluate_expr(val.second);
                   return Value::binop(val.op, first_val, second_val);
                 },
                 [&](MR::AssignExpr &val) -> Value {
                   auto second_val = evaluate_expr(val.second);

                   if (auto path = mr.get_expr(val.first)) {
                     if (std::holds_alternative<MR::PathExpr>(path->kind)) {
                       auto &path_expr = std::get<MR::PathExpr>(path->kind);
                       set_variable(path_expr.path.to_string(), second_val);
                       return second_val;
                     }
                   }
                   throw RuntimeError("Invalid assignment target");
                 },
                 [&](MR::AssignOpExpr &val) -> Value {
                   auto first_val = evaluate_expr(val.first);
                   auto second_val = evaluate_expr(val.second);
                   auto result = Value::binop(val.op, first_val, second_val);

                   if (auto path = mr.get_expr(val.first)) {
                     if (std::holds_alternative<MR::PathExpr>(path->kind)) {
                       auto &path_expr = std::get<MR::PathExpr>(path->kind);
                       set_variable(path_expr.path.to_string(), second_val);
                       return second_val;
                     }
                   }
                   throw RuntimeError("Invalid assignment target");
                 },
                 // Add other expression handlers
                 [&](auto &) -> Value {
                   throw std::runtime_error("Unhandled expression type");
                 }},
      node->kind);
}

Value Environment::evaluate_stmt(MR::Id &stmt) {
  auto node = mr.get_stmt(stmt);
  if (!node) {
    throw std::runtime_error("Statement node not found");
  }
  return std::visit(
      overloaded{
          [&](MR::ExprStmt &val) -> Value {
            return evaluate_expr(val.expr);
          },
          [&](MR::LetStmt &val) -> Value {
            if (val.initializer.has_value()) {
              auto init_val = evaluate_expr(val.initializer.value());
              if (std::holds_alternative<MR::IdentPat>(val.pattern.kind)) {
                auto &ident = std::get<MR::IdentPat>(val.pattern.kind);
                declare_variable(ident.identifier, ident.binding.isMutable());
                set_variable(ident.identifier, init_val);
              } else {
                throw std::runtime_error("invalid identifier in let stmt");
              }
            }
            return Value(); // return unit
          },
          // Add other statement handlers
          [&](MR::PrintStmt &val) -> Value {
              auto print_val = evaluate_expr(val.expr);
              std::cout << "[MRC::OUTPUT] " << print_val.to_string() << std::endl;
              return Value(); // return unit
          },
          [&](auto &) -> Value {
            throw std::runtime_error("Unhandled statement type");
          }},
      node->kind);
}

Value Environment::evaluate_block(MR::Id &block) {
  auto node = mr.get_block(block);
  if (!node) {
    throw std::runtime_error("Block node not found");
  }

  Value last_value{};
  for (auto &stmt : node->statements) {
    last_value = evaluate_stmt(stmt);
  }
  return last_value;
}

Interpreter::Interpreter(P<TS::TypeContext> context, MR::Mr &mr)
    : type_context(context), mr(mr) {
  this->environment = std::make_shared<Environment>(Environment(mr));
}

void Interpreter::interp(std::string &entry) {
  auto fn_entry = mr.tree->lookup(entry);

  Environment root = Environment(nullptr, mr, mr.tree);

  if (fn_entry.has_value()) {
    root.evaluate_fn(fn_entry.value()->id);
  }

  root.print();
}

} // namespace MRC::INTERP
