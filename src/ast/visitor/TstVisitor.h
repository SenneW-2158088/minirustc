#include "ast/visitor/Visitor.h"
#include "util/util.h"

#include "tst/tst.h"

namespace MRC::AST {

// General struct for building a tst
struct TstBuilder {
  // Build TST from an Ast
  void build(Ast &ast) {

  }
};

struct TstVisitor : Visitor {
    // Only necessary to visit expr, stmts, items, blocks since
    // we can parse them directly and need their context
    void visit_stmt(Stmt &) override {

    }
};
} // namespace MRC::AST
