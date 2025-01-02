#pragma once

#include "typechecking/Type.h"
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

  MRC::TS::Type to_type() const {
      return std::visit(overloaded{
        [&](const InferType&) ->MRC::TS::Type { return MRC::TS::Type::MakeUnset(); },
        [&](const PathType& val) -> MRC::TS::Type {
            auto path = to_string();
            // Integers
            if(path == "i8") return MRC::TS::Type::makeInt(true, 8);
            if(path == "i16") return MRC::TS::Type::makeInt(true, 16);
            if(path == "i32") return MRC::TS::Type::makeInt(true, 32);
            if(path == "i64") return MRC::TS::Type::makeInt(true, 64);

            if(path == "u8") return MRC::TS::Type::makeInt(false, 8);
            if(path == "u16") return MRC::TS::Type::makeInt(false, 16);
            if(path == "u32") return MRC::TS::Type::makeInt(false, 32);
            if(path == "u64") return MRC::TS::Type::makeInt(false, 64);

            // Floats
            if(path == "f8") return MRC::TS::Type::makeFloat(8);
            if(path == "f16") return MRC::TS::Type::makeFloat(16);
            if(path == "f32") return MRC::TS::Type::makeFloat(32);
            if(path == "f64") return MRC::TS::Type::makeFloat(64);

            // Boolean
            if(path == "bool") return MRC::TS::Type::makeBool();
            if(path == "str") return MRC::TS::Type::makeString();

            return MRC::TS::Type::MakeUnset();
        }
      }, kind);
  }

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
