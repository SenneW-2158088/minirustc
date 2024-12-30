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

  std::string to_string() const {
    std::string result;
    std::visit(overloaded{
      [&result](const InferType&) {
        result = "infer";
      },
      [&result](const PathType& val) {
        result = val.path.get()->segments.at(0);
        for(int i = 1; i < val.path.get()->segments.size(); ++i) {
          result += ":" + val.path.get()->segments[i];
        }
      }
    }, kind);
    return result;
  }
};
} // namespace MRC::AST
