#pragma once

#include <optional>
namespace MRC::AST {
  struct BindingMode {

    enum class Mutability {
      NONE,
      MUT
    };

    Mutability mutability;
    std::optional<Mutability> byRef;

    bool isMutable() {
      if(byRef.has_value()){
        return byRef.value() == Mutability::MUT;
      }
      return mutability == Mutability::MUT;
    }
  };
}
