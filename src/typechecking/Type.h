#pragma once

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
};

struct VarType { Type type; };

struct ConcreteType { Type type; };

struct CheckType {
    using TypeKind = std::variant<VarType, ConcreteType>;
    TypeKind kind;

    explicit CheckType(TypeKind kind) : kind(std::move(kind)) {}
    static CheckType makeVar(Type type) { return CheckType(VarType{ .type = std::move(type) });};
    static CheckType makeConcrete(Type type) { return CheckType(ConcreteType{ .type = std::move(type) });};

    std::string to_string() {
        return std::visit( overloaded{
            [&](VarType &val) -> std::string {
                return "varType:" +val.type.to_string();
            },
            [&](ConcreteType &val) -> std::string {
                return "ConcreteType:" + val.type.to_string();
            }
        }, this->kind);
    }
};

} // namespace MRC::HIR
