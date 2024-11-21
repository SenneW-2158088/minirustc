#pragma once

#include <optional>
namespace MRC::AST {
struct BindingMode {

  enum class Mutability { NONE, MUT };

  Mutability mutability;
  std::optional<Mutability> byRef;

public:
  BindingMode() = default;
  BindingMode(Mutability mutability, std::optional<Mutability> byRef = std::nullopt)
      : mutability(std::move(mutability)), byRef(std::move(byRef)) {}

  static BindingMode make() {
    return BindingMode(Mutability::NONE);
  }

  static BindingMode makeMut() {
    return BindingMode(Mutability::MUT);
  }

  bool isMutable() {
    if (byRef.has_value()) {
      return byRef.value() == Mutability::MUT;
    }
    return mutability == Mutability::MUT;
  }
};
} // namespace MRC::AST
