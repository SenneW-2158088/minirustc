#pragma once

#include <string>
#include <variant>
#include <vector>
#include <iostream>

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
};

struct Type {
  using TypeKind = std::variant<IntType, FloatType, BoolType, VoidType,
                                StringType, FunctionType, UnsetType>;

  TypeKind kind;

  Type(TypeKind kind) : kind(std::move(kind)) {}

  static Type makeInt(bool is_signed, uint8_t bits) {
    return Type(IntType{is_signed, bits});
  }

  static Type makeFloat(uint8_t bits) { return Type(FloatType{bits}); }

  static Type makeVoid() { return Type(VoidType{}); }

  static Type makeBool() { return Type(BoolType{}); }

  static Type MakeUnset() { return Type(UnsetType{}); }

  static Type makeString() { return Type(StringType{}); }

  static Type makeFunction(std::vector<U<Type>> params, U<Type> return_type) {
    return Type(FunctionType{std::move(params), std::move(return_type)});
  }

  bool equals(const Type &other) const;

  std::string to_string() const;

  bool unionize(Type &other) {
    return std::visit(overloaded{
        [&](IntType &left, IntType &right) -> bool {
            if (left.bits >= right.bits) {
                if (left.is_signed == right.is_signed) {
                    return true;
                }
            }
            return false;
        },
        [&](FloatType &left, FloatType &right) -> bool {
            return left.bits >= right.bits;
        },
        [&](UnsetType &left, auto &right) -> bool { return true; },
        [&](auto &left, auto &right) -> bool {
            return typeid(left) == typeid(right);
        }
    }, this->kind, other.kind);
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

  explicit CheckType(TypeKind kind) : kind(std::move(kind)) {}
  static CheckType makeVar(Type type) {
    return CheckType(VarType{.type = std::move(type)});
  };
  static CheckType makeConcrete(Type type) {
    return CheckType(ConcreteType{.type = std::move(type)});
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
      std::cout << "Unionize " << this->to_string() << " with " << other.to_string() << std::endl;
      return std::visit(
        overloaded{
            [&](VarType &val, VarType &other_val) -> bool {
              if (std::holds_alternative<UnsetType>(val.type.kind)) {
                val.type = std::move(other_val.type);
                return true;
              }
              return val.type.unionize(other_val.type);
            },
            [&](VarType &val, ConcreteType &other_val) -> bool {
              if (std::holds_alternative<UnsetType>(val.type.kind)) {
                this->kind = ConcreteType(std::move(other_val.type));
                return true;
              }
              if(val.type.unionize(other_val.type)) {
                this->kind = ConcreteType(std::move(other_val.type));
                return true;
              }
              return false;
            },
            [&](ConcreteType &val, VarType &other_val) -> bool {
              if (std::holds_alternative<UnsetType>(other_val.type.kind)) {
                  return true;
              }
              if(other_val.type.unionize(val.type)) {
                other.kind = ConcreteType(std::move(val.type));
                return true;
              }
              return false;
            },
            [&](ConcreteType &val, ConcreteType &other_val) -> bool {
              return val.type.unionize(other_val.type);
            }},
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
