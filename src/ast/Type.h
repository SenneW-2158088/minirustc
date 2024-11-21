#pragma once

#include "util/util.h"
#include "ast/prelude.h"

namespace MRC::AST {

struct InferType{};
struct PathType {
  U<Path> path;

public:
  PathType() = default;
  PathType(U<Path> path) : path(std::move(path)) {}
};
struct Type {
  using TypeKind = std::variant<InferType, PathType>;
  TypeKind kind;

public:
  Type() = default;
  Type(TypeKind kind) : kind(std::move(kind)) {}

  static Type makeInfer() { return Type(InferType());}
  static Type makePath(U<Path> path) { return Type(PathType(std::move(path)));}
};
} // namespace MRC::AST
