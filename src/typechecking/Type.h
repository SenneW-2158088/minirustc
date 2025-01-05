#pragma once

#include <iostream>
#include <string>
#include <variant>
#include <vector>

#include "util/util.h"

namespace MRC::TS {
struct Type;

struct IntType {
  bool is_signed;
  uint8_t bits; // 8, 16, 32, 64
};

struct FloatType {
  uint8_t bits; // 32, 64
};

struct BoolType {};
struct StringType {};
struct UnsetType {};
struct VoidType {};

struct FunctionType {
  std::vector<U<Type>> params;
  U<Type> return_type;
  FunctionType() = default;

  // Constructor with parameters
  FunctionType(std::vector<U<Type>> params, U<Type> return_type)
      : params(std::move(params)), return_type(std::move(return_type)) {}

  FunctionType(const FunctionType &other) {
    // Deep copy params vector
    params.reserve(other.params.size());
    for (const auto &param : other.params) {
      params.push_back(std::make_unique<Type>(*param));
    }

    // Deep copy return type
    if (other.return_type) {
      return_type = std::make_unique<Type>(*other.return_type);
    }
  }

  // Copy assignment operator
  FunctionType &operator=(const FunctionType &other) {
    if (this != &other) {
      // Clear existing params
      params.clear();

      // Deep copy params vector
      params.reserve(other.params.size());
      for (const auto &param : other.params) {
        params.push_back(std::make_unique<Type>(*param));
      }

      // Deep copy return type
      if (other.return_type) {
        return_type = std::make_unique<Type>(*other.return_type);
      } else {
        return_type = nullptr;
      }
    }
    return *this;
  }

  // Move constructor and assignment can be defaulted
  FunctionType(FunctionType &&) = default;
  FunctionType &operator=(FunctionType &&) = default;
};

struct Type {
  using TypeKind = std::variant<IntType, FloatType, BoolType, VoidType,
                                StringType, FunctionType, UnsetType>;

  TypeKind kind;

  Type(TypeKind kind) : kind(kind) {}

  static Type makeInt(bool is_signed, uint8_t bits) {
    return Type(IntType{is_signed, bits});
  }

  static Type makeFloat(uint8_t bits) { return Type(FloatType{bits}); }

  static Type makeVoid() { return Type(VoidType{}); }

  static Type makeBool() { return Type(BoolType{}); }

  static Type makeUnset() { return Type(UnsetType{}); }

  static Type makeString() { return Type(StringType{}); }

  static Type makeFunction(std::vector<U<Type>> params, U<Type> return_type) {
    return Type(FunctionType{std::move(params), std::move(return_type)});
  }

  bool equals(const Type &other) const;

  std::string to_string() const;

  bool unionize(Type &other) {
    return std::visit(
        overloaded{[&](IntType &left, IntType &right) -> bool {
                     if (left.bits >= right.bits) {
                       if (left.is_signed == right.is_signed) {
                         right = left; // Promote right to the wider type
                         return true;
                       }
                     } else {
                       if (left.is_signed == right.is_signed) {
                         left = right; // Promote left to the wider type
                         return true;
                       }
                     }
                     return false;
                   },
                   [&](FloatType &left, FloatType &right) -> bool {
                     if (left.bits >= right.bits) {
                       right = left;
                       return true;
                     } else {
                       left = right;
                       return true;
                     }
                   },
                   [&](FunctionType &left, FunctionType &right) -> bool {
                     if(left.params.size() != right.params.size()) return false;
                     if(!left.return_type->equals(*right.return_type)) return false;

                     for(int i = 0; i < left.params.size(); ++i) {
                       if(!left.params[i]->unionize(*right.params[i])) return false;
                     }

                     return true;
                   },
                   [&](auto &left, UnsetType &right) -> bool {
                     return true;
                   },
                   [&](auto &left, auto &right) -> bool {
                     return typeid(left) == typeid(right);
                   }}, this->kind, other.kind);
  }
};

struct VarType {
  Type type;
};

struct ConcreteType {
  Type type;
};

struct CheckType {
  using TypeKind = std::variant<VarType, ConcreteType>;
  TypeKind kind;

  explicit CheckType(TypeKind kind) : kind(kind) {}
  static CheckType makeVar(Type type) {
    return CheckType(VarType{.type = type});
  };
  static CheckType makeConcrete(Type type) {
    return CheckType(ConcreteType{.type = type});
  };

  bool equals(CheckType &other) {
    return std::visit(
        overloaded{[&](VarType &val, ConcreteType &other) -> bool {
                     return val.type.equals(other.type);
                   },
                   [&](VarType &val, VarType &other) -> bool {
                     return val.type.equals(other.type);
                   },
                   [&](ConcreteType &val, ConcreteType &other) -> bool {
                     return val.type.equals(other.type);
                   },
                   [&](auto &val, auto &other) -> bool {
                     return val.type.equals(other.type);
                   }},
        this->kind, other.kind);
  }

  bool is_variant() const { return std::holds_alternative<VarType>(kind); }

  bool is_concrete() const {
    return std::holds_alternative<ConcreteType>(kind);
  }

  bool unionize(CheckType &other) {
    // std::cout << "Unionize " << this->to_string() << " with "
    //           << other.to_string() << std::endl;
    return std::visit(
        overloaded{[&](VarType &val, VarType &other_val) -> bool {
                     if (val.type.unionize(other_val.type)) {
                       other.kind = VarType(val.type);
                       return true;
                     }
                     return false;
                   },
                   [&](ConcreteType &val, VarType &other_val) -> bool {
                     if (val.type.unionize(other_val.type)) {
                       other.kind = ConcreteType(val.type);
                       return true;
                     }
                     return false;
                   },
                   [&](ConcreteType &val, ConcreteType &other_val) -> bool {
                     return val.type.equals(other_val.type);
                   },
                   [&](auto &val, auto &other_val) -> bool { return false; }},
        this->kind, other.kind);
  }
  std::string to_string() {
    return std::visit(overloaded{[&](VarType &val) -> std::string {
                                   return "varType:" + val.type.to_string();
                                 },
                                 [&](ConcreteType &val) -> std::string {
                                   return "ConcreteType:" +
                                          val.type.to_string();
                                 }},
                      this->kind);
  }
};
} // namespace MRC::TS
