#pragma once

// Structure for representing patterns
#include "ast/Ident.h"
#include "ast/struct/BindingMode.h"
#include <optional>
#include <variant>

namespace MRC::AST {

struct Pat;

struct IdentPat {
  BindingMode binding;
  Ident identifier;
  Opt<U<Pat>> pattern;

public:
  IdentPat() = default;
  IdentPat(BindingMode binding, Ident identifier, Opt<U<Pat>> pattern)
      : binding(std::move(binding)), identifier(std::move(identifier)),
        pattern(std::move(pattern)) {}
};

struct Pat {
  using PatKind = std::variant<IdentPat>;
  PatKind kind;

public:
  Pat() = default;

  explicit Pat(PatKind kind) : kind(std::move(kind)) {}

  static Pat makeIdent(BindingMode binding, Ident identifier,
                       Opt<U<Pat>> pattern = std::nullopt) {
    return Pat(IdentPat(std::move(binding), std::move(identifier),
                        std::move(pattern)));
  }
};
} // namespace MRC::AST::STRUCT
