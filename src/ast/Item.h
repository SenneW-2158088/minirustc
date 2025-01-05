#pragma once

#include "ast/prelude.h"
#include <variant>

namespace MRC::AST {
struct Item;

struct FnItem {
  U<Fn> fn;
  FnItem() = default;
  FnItem(U<Fn> fn) : fn(std::move(fn)) {}
};

struct Item {
  using ItemKind = std::variant<FnItem>;
  Ident ident;
  ItemKind kind;
  Id id;

  Item() = default;

  explicit Item(Id id, Ident ident, ItemKind kind)
      : id(id), ident(std::move(ident)), kind(std::move(kind)) {}
  static Item makeFn(Id id, Ident ident, U<Fn> fn) {
    return Item(id, std::move(ident), FnItem(std::move(fn)));
  }
};
} // namespace MRC::AST
