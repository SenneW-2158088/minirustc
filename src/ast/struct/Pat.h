#pragma once

// Structure for representing patterns
#include "ast/Expr.h"
#include "ast/Ident.h"
#include "ast/Lit.h"
#include "ast/struct/BindingMode.h"
#include <optional>
#include <variant>

namespace MRC::AST {

struct Expr; // Forward declare
struct Pat;  // Forward declare

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
    return Pat(IdentPat(std::move(binding), std::move(identifier),
                        std::move(pattern)));
  }
  static Pat makeLit(U<Expr> expr) { return Pat(LitPat(std::move(expr))); }
};
} // namespace MRC::AST
