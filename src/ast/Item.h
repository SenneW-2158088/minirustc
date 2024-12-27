#pragma once

#include "ast/prelude.h"

namespace MRC::AST {
struct Item;

struct FnItem {
  U<Fn> fn;
  FnItem() = default;
  FnItem(U<Fn> fn) : fn(std::move(fn)) {}
};

// To be supported later
// Enum
// Struct
// Const

struct Item {
  using ItemKind = std::variant<FnItem>;
  Ident ident;
  ItemKind kind;
  Item() = default;
  explicit Item(Ident ident, ItemKind kind) : ident(std::move(ident)), kind(std::move(kind)) {}
  static Item makeFn(Ident ident, U<Fn> fn) { return Item(std::move(ident), FnItem(std::move(fn))); }
};
} // namespace MRC::AST
