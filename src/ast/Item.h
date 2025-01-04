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

// To be supported later
// Enum
// Struct
// Const

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

  Body lower() {
    return std::visit(
        overloaded{[&](FnItem &fn_item) -> Body {
          if (fn_item.fn->body) {
            auto block_expr = MU<Expr>(Expr::makeBlock(id, std::move(fn_item.fn->body.value())));
            return Body(id, std::move(fn_item.fn->params), std::move(block_expr)); }
          else {
            auto empty_block = std::make_unique<Block>();
            auto block_expr = MU<Expr>(Expr::makeBlock(id, std::move(empty_block)));
            return Body(id, std::move(fn_item.fn->params), std::move(block_expr));
          }
        }},
        kind);
  }

};
} // namespace MRC::AST
