#pragma once

#include "Token.h"
#include "ast/Type.h"
#include "prelude.h"
#include <map>
#include <vector>

namespace MRC::TST {

struct Entry {
  Symbol symbol;
  AST::Type type;
};

struct Context {
  std::map<Symbol, Entry> symbol_table;
};

// Struct representing a type
struct Ty {};

struct Stmt {

};

struct Expr {
    using ExprKind = std::variant<int>;
    ExprKind kind;
    Ty type;
};

struct Tst {
    std::vector<int> statements;
    std::vector<int> blocks;
    std::vector<int> params;
    std::vector<int> expressions;

  using Index = int;
  Index insert_stmt();
};

// Trying some cute lil C style syntax
void insert_entry(Context &context, Entry entry);
void print_context(Context &context);

} // namespace MRC::TST
