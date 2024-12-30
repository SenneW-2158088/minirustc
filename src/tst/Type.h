#pragma once

#include <string>
#include <variant>
#include <vector>

#include "prelude.h"
#include "util/util.h"

namespace MRC::TST {

struct IntType {
  bool is_signed;
  uint8_t bits; // 8, 16, 32, 64
};

struct FloatType {
  uint8_t bits; // 32, 64
};

struct BoolType {};
struct StringType {};

struct FunctionType {
  std::vector<U<Type>> params;
  U<Type> return_type;
};

// The main Type class holding all possible types
struct Type {
  using TypeKind = std::variant<IntType, FloatType, BoolType,
                                StringType, FunctionType>;

  TypeKind kind;

  Type(TypeKind kind) : kind(std::move(kind)) {}

  static Type makeInt(bool is_signed, uint8_t bits) {
    return Type(IntType{is_signed, bits});
  }

  static Type makeFloat(uint8_t bits) { return Type(FloatType{bits}); }

  static Type makeBool() { return Type(BoolType{}); }


  static Type makeString() { return Type(StringType{}); }

  static Type makeFunction(std::vector<U<Type>> params, U<Type> return_type) {
    return Type(FunctionType{std::move(params), std::move(return_type)});
  }

  bool equals(const Type &other) const;

  std::string toString() const;
};

} // namespace MRC::HIR
