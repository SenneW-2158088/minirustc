#pragma once

namespace MRC::AST {

    struct IntegerLit {
       int value;
    };

   struct Lit {
        std::variant<IntegerLit> kind;
   };
}
