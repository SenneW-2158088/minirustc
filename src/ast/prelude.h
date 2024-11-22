#pragma once

namespace MRC::AST {
struct Ast;
struct Stmt;
struct Expr;
struct Iden;
struct Lit;
struct Fn;
struct Item;
struct Type;
struct Local;
struct Pat;
struct Path;
struct Block;
struct BindingMode;
} // namespace MRC::AST

#include "ast/Expr.h"
#include "ast/Ident.h"
#include "ast/Lit.h"
#include "ast/Stmt.h"
#include "ast/Type.h"
#include "ast/Item.h"
#include "ast/struct/BindingMode.h"
#include "ast/struct/Block.h"
#include "ast/struct/Local.h"
#include "ast/struct/Pat.h"
#include "ast/struct/Path.h"
#include "ast/struct/Fn.h"
#include "ast/visitor/Visitor.h"
