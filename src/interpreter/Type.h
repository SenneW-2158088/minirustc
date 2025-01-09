#pragma once

#include "ast/Binop.h"
#include "mr/prelude.h"
#include "util/util.h"
#include <variant>

namespace MRC::INTERP {
struct Function;
struct Unit{};
struct Value {
  using ValueKind = std::variant<Unit, bool, int, double, std::string,
                                 std::shared_ptr<Function>>;

  ValueKind kind;
  Value() : kind(Unit{}) {}
  template <typename T> Value(T value) : kind(std::move(value)) {}
  Value(MR::Lit &lit);
  bool is_unit() const { return std::holds_alternative<Unit>(kind); }
  bool is_bool() const { return std::holds_alternative<bool>(kind); }
  bool is_int() const { return std::holds_alternative<int>(kind); }
  bool is_float() const { return std::holds_alternative<double>(kind); }
  bool is_string() const { return std::holds_alternative<std::string>(kind); }
  bool is_function() const {
    return std::holds_alternative<std::shared_ptr<Function>>(kind);
  }

  // Value extraction helpers
  template <typename T> T &as() { return std::get<T>(kind); }

  template <typename T> const T &as() const { return std::get<T>(kind); }

  // String representation
  std::string to_string() const {
    return std::visit(
        overloaded{[](Unit) -> std::string { return "()"; },
                   [](bool b) -> std::string { return b ? "true" : "false"; },
                   [](int i) -> std::string { return std::to_string(i); },
                   [](double d) -> std::string { return std::to_string(d); },
                   [](const std::string &s) -> std::string { return s; },
                   [](const std::shared_ptr<Function> &) -> std::string {
                     return "<function>";
                   }},
        kind);
  }

  static Value binop(AST::BinOp op, Value &left, Value& right);
};

struct Variable {
  Opt<Value> value;
  bool is_mutable;

  bool is_initialized() { return value.has_value(); }
  Variable() = default;
};
} // namespace MRC::INTERP
