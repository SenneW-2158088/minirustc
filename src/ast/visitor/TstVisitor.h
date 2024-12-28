#include "ast/visitor/Visitor.h"
#include "util/util.h"

#include "tst/tst.h"

namespace MRC::AST {

// General struct for building a tst
struct TstBuilder {
  // Build TST from an Ast
  TST::Tst build(Ast &ast) {
      TST::Tst tst{};

  }
};
struct TstVisitor : Visitor {};
} // namespace MRC::AST
