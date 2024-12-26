#pragma once

#include "ast/prelude.h"

namespace MRC::AST {

struct IdentPat {
  BindingMode binding;
  Ident identifier;
  Opt<U<Pat>> pattern;

public:
  IdentPat() = default;
  explicit IdentPat(BindingMode binding, Ident identifier, Opt<U<Pat>> pattern)
      : binding(std::move(binding)), identifier(std::move(identifier)),
        pattern(std::move(pattern)) {}
};

struct LitPat {
  U<Expr> expr;

public:
  LitPat() = default;
  explicit LitPat(U<Expr> expr) : expr(std::move(expr)) {}
};

struct Pat {
  using PatKind = std::variant<IdentPat, LitPat>;
  PatKind kind;

public:
  Pat() = default;

  explicit Pat(PatKind kind) : kind(std::move(kind)) {}

  static Pat makeIdent(BindingMode binding, Ident identifier,
                       Opt<U<Pat>> pattern = std::nullopt) {
    return Pat(std::move(IdentPat(std::move(binding), std::move(identifier),
                                  std::move(pattern))));
  }
  static Pat makeLit(U<Expr> expr) {
    return Pat(std::move(LitPat(std::move(expr))));
  }
};
} // namespace MRC::AST
