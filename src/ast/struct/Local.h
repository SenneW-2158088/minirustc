#pragma once

#include "ast/prelude.h"
#include "typechecking/Type.h"

namespace MRC::AST {
struct Stmt;
struct Block;

struct DeclLocal {};

struct InitLocal {
  U<Expr> expr;
};

struct InitElseLocal {
  U<Expr> expr;
  U<Block> block;
};

struct Local {
  using LocalKind = std::variant<DeclLocal, InitLocal, InitElseLocal>;
  U<Pat> pat;
  LocalKind kind;
  Opt<U<Type>> type;
  TS::CheckType check_type;
  Id id;

public:
explicit Local(Id id, U<Pat> pat, Opt<U<Type>> type, LocalKind kind)
    : kind(std::move(kind))
    , id(id)
    , pat(std::move(pat))
    , type({})  // Initialize with empty optional first
    , check_type(TS::CheckType::makeVar(TS::Type::makeUnset()))  // Initialize with default
{
    if (type.has_value()) {
        auto concrete_type = type.value()->to_type();
        this->check_type = TS::CheckType(TS::CheckType::makeConcrete(std::move(concrete_type)));
        this->type = std::move(type);  // Move type after we're done using it
    }
}

  static Local makeDecl(Id id, U<Pat> pat, Opt<U<Type>> type) {
    return Local(id, std::move(pat), std::move(type), DeclLocal());
  }
  static Local makeInit(Id id, U<Pat> pat, Opt<U<Type>> type, U<Expr> expr) {
    return Local(id, std::move(pat), std::move(type), InitLocal(std::move(expr)));
  }
  static Local makeInitElse(Id id, U<Pat> pat, Opt<U<Type>> type, U<Expr> expr,
                            U<Block> block) {
    return Local(id, std::move(pat), std::move(type),
                 InitElseLocal(std::move(expr), std::move(block)));
  }
};
} // namespace MRC::AST
