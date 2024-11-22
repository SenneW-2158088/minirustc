#pragma once

#include "ast/prelude.h"

namespace MRC::AST {
struct Item;

struct FnKind {
  U<Fn> fn;
  FnKind() = default;
  FnKind(U<Fn> fn) : fn(std::move(fn)) {}
};

// To be supported later
// Enum
// Struct
// Const

struct Item {
  using ItemKind = std::variant<FnKind>;
  Ident ident;
  ItemKind kind;
  Item() = default;
  explicit Item(Ident ident, ItemKind kind) : kind(std::move(kind)) {}
  static Item makeFn(Ident ident, U<Fn> fn) { return Item(std::move(ident), FnKind(std::move(fn))); }
};
} // namespace MRC::AST
