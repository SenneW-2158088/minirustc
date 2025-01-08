#pragma once
#include "util/util.h"
#include <string>
#include <variant>
#include <vector>

namespace MRC::TS {
struct Type;
struct CheckType;

struct IntType {
  bool is_signed;
  uint8_t bits;
};

struct FloatType {
  uint8_t bits;
};

struct BoolType {};
struct StringType {};
struct UnsetType {};
struct VoidType {};

struct FunctionType {
  std::vector<U<CheckType>> params;
  U<CheckType> return_type;

  FunctionType();
  FunctionType(std::vector<U<CheckType>> params, U<CheckType> return_type);
  FunctionType(const FunctionType &other);
  FunctionType &operator=(const FunctionType &other);
};

struct Type {
  using TypeKind = std::variant<IntType, FloatType, BoolType, VoidType,
                                StringType, FunctionType, UnsetType>;
  TypeKind kind;

  Type() : kind(VoidType()){}
  Type(TypeKind kind);
  static Type makeInt(bool is_signed, uint8_t bits);
  static Type makeFloat(uint8_t bits);
  static Type makeVoid();
  static Type makeBool();
  static Type makeUnset();
  static Type makeString();
  static Type makeFunction(std::vector<U<CheckType>> params,
                           U<CheckType> return_type);

  bool equals(const Type &other) const;
  std::string to_string() const;
  bool unionize(Type &other);
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

  explicit CheckType(TypeKind kind);
  static CheckType makeVar(Type type);
  static CheckType makeConcrete(Type type);

  bool equals(CheckType &other);
  bool is_variant() const;
  bool is_concrete() const;
  bool unionize(CheckType &other);
  std::string to_string();
};
} // namespace MRC::TS
