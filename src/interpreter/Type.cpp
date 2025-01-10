#include "interpreter/Type.h"
#include "ast/Binop.h"
#include "mr/prelude.h"
#include <stdexcept>

namespace MRC::INTERP {
  Value::Value(MR::Lit &lit) {
           std::visit(overloaded{
            [&](const MR::IntegerLit&) {
                try {
                    kind = std::stoi(lit.symbol);
                } catch (const std::exception& e) {
                    throw std::runtime_error("Invalid integer literal: " + lit.symbol);
                }
            },
            [&](const MR::BooleanLit&) {
                if (lit.symbol == "true") {
                    kind = true;
                } else if (lit.symbol == "false") {
                    kind = false;
                } else {
                    throw std::runtime_error("Invalid boolean literal: " + lit.symbol);
                }
            },
            [&](const MR::FloatLit&) {
                try {
                    kind = std::stod(lit.symbol);
                } catch (const std::exception& e) {
                    throw std::runtime_error("Invalid float literal: " + lit.symbol);
                }
            },
            [&](const MR::StrLit&) {
                std::string str = lit.symbol;
                if (str.size() >= 2 && str.front() == '"' && str.back() == '"') {
                    str = str.substr(1, str.size() - 2);
                }
                kind = str;
            }
        }, lit.kind);
  }

  Value Value::unop(AST::UnOp op, Value &val) {
      return std::visit(overloaded{
          [&](const AST::Not &) {
              if (val.is_bool()) {
                  return Value(!val.as<bool>());
              }
              if (val.is_int()) {
                  return Value(~val.as<int>());  // Bitwise NOT for integers
              }
              throw std::runtime_error("Invalid type for NOT operation");
          }
      }, op.kind);
  }

  Value Value::binop(AST::BinOp op, Value &left, Value &right) {
        return std::visit(overloaded{
            [&](const AST::Add&) {
                if (left.is_int() && right.is_int()) {
                    return Value(left.as<int>() + right.as<int>());
                }
                if (left.is_float() && right.is_float()) {
                    return Value(left.as<double>() + right.as<double>());
                }
                if (left.is_string() && right.is_string()) {
                    return Value(left.as<std::string>() + right.as<std::string>());
                }
                throw std::runtime_error("Invalid types for addition");
            },
            [&](const AST::Sub&) {
                if (left.is_int() && right.is_int()) {
                    return Value(left.as<int>() - right.as<int>());
                }
                if (left.is_float() && right.is_float()) {
                    return Value(left.as<double>() - right.as<double>());
                }
                throw std::runtime_error("Invalid types for subtraction");
            },
            [&](const AST::Mul&) {
                if (left.is_int() && right.is_int()) {
                    return Value(left.as<int>() * right.as<int>());
                }
                if (left.is_float() && right.is_float()) {
                    return Value(left.as<double>() * right.as<double>());
                }
                throw std::runtime_error("Invalid types for multiplication");
            },
            [&](const AST::Div&) {
                if (right.is_int()) {
                    if (right.as<int>() == 0) throw std::runtime_error("Division by zero");
                    if (left.is_int()) {
                        return Value(left.as<int>() / right.as<int>());
                    }
                }
                if (right.is_float()) {
                    if (right.as<double>() == 0.0) throw std::runtime_error("Division by zero");
                    if (left.is_float()) {
                        return Value(left.as<double>() / right.as<double>());
                    }
                }
                throw std::runtime_error("Invalid types for division");
            },
            [&](const AST::Mod&) {
                if (left.is_int() && right.is_int()) {
                    if (right.as<int>() == 0) throw std::runtime_error("Modulo by zero");
                    return Value(left.as<int>() % right.as<int>());
                }
                throw std::runtime_error("Invalid types for modulo operation");
            },
            [&](const AST::And&) {
                if (left.is_bool() && right.is_bool()) {
                    return Value(left.as<bool>() && right.as<bool>());
                }
                throw std::runtime_error("Invalid types for logical AND");
            },
            [&](const AST::Or&) {
                if (left.is_bool() && right.is_bool()) {
                    return Value(left.as<bool>() || right.as<bool>());
                }
                throw std::runtime_error("Invalid types for logical OR");
            },
            [&](const AST::Eq&) {
                if (left.is_int() && right.is_int()) {
                    return Value(left.as<int>() == right.as<int>());
                }
                if (left.is_float() && right.is_float()) {
                    return Value(left.as<double>() == right.as<double>());
                }
                if (left.is_bool() && right.is_bool()) {
                    return Value(left.as<bool>() == right.as<bool>());
                }
                if (left.is_string() && right.is_string()) {
                    return Value(left.as<std::string>() == right.as<std::string>());
                }
                throw std::runtime_error("Invalid types for equality comparison");
            },
            [&](const AST::Ne&) {
                if (left.is_int() && right.is_int()) {
                    return Value(left.as<int>() != right.as<int>());
                }
                if (left.is_float() && right.is_float()) {
                    return Value(left.as<double>() != right.as<double>());
                }
                if (left.is_bool() && right.is_bool()) {
                    return Value(left.as<bool>() != right.as<bool>());
                }
                if (left.is_string() && right.is_string()) {
                    return Value(left.as<std::string>() != right.as<std::string>());
                }
                throw std::runtime_error("Invalid types for inequality comparison");
            },
            [&](const AST::Lt&) {
                if (left.is_int() && right.is_int()) {
                    return Value(left.as<int>() < right.as<int>());
                }
                if (left.is_float() && right.is_float()) {
                    return Value(left.as<double>() < right.as<double>());
                }
                if (left.is_string() && right.is_string()) {
                    return Value(left.as<std::string>() < right.as<std::string>());
                }
                throw std::runtime_error("Invalid types for less than comparison");
            },
            [&](const AST::Le&) {
                if (left.is_int() && right.is_int()) {
                    return Value(left.as<int>() <= right.as<int>());
                }
                if (left.is_float() && right.is_float()) {
                    return Value(left.as<double>() <= right.as<double>());
                }
                if (left.is_string() && right.is_string()) {
                    return Value(left.as<std::string>() <= right.as<std::string>());
                }
                throw std::runtime_error("Invalid types for less than or equal comparison");
            },
            [&](const AST::Gt&) {
                if (left.is_int() && right.is_int()) {
                    return Value(left.as<int>() > right.as<int>());
                }
                if (left.is_float() && right.is_float()) {
                    return Value(left.as<double>() > right.as<double>());
                }
                if (left.is_string() && right.is_string()) {
                    return Value(left.as<std::string>() > right.as<std::string>());
                }
                throw std::runtime_error("Invalid types for greater than comparison");
            },
            [&](const AST::Ge&) {
                if (left.is_int() && right.is_int()) {
                    return Value(left.as<int>() >= right.as<int>());
                }
                if (left.is_float() && right.is_float()) {
                    return Value(left.as<double>() >= right.as<double>());
                }
                if (left.is_string() && right.is_string()) {
                    return Value(left.as<std::string>() >= right.as<std::string>());
                }
                throw std::runtime_error("Invalid types for greater than or equal comparison");
            }
        }, op.kind);
    }
}
