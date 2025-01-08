#include "mr/Mr.h"
#include "ast/Ast.h"
#include "ast/Expr.h"
#include "ast/struct/Fn.h"
#include "ast/struct/Local.h"
#include "ast/struct/Path.h"
#include "mr/prelude.h"
#include "typechecking/Type.h"
#include "typechecking/TypeChecker.h"
#include "util/util.h"
#include <memory>
#include <optional>
#include <variant>

namespace MRC::MR {

TS::Type resolve_type(int id, TS::TypeContext &context) {
  auto *t = context.resolve(id);
  return std::visit(
      overloaded{[&](TS::VarType &val) -> TS::Type { return val.type; },
                 [&](TS::ConcreteType &val) -> TS::Type { return val.type; }},
      t->kind);
}

Lit MrBuilder::make_lit(AST::Lit &lit) {
  Lit result;
  result.symbol = lit.symbol;
  result.type = resolve_type(lit.id, *type_context);

  std::visit(
      overloaded{
          [](AST::IntegerLit &) -> Lit::LiteralKind { return IntegerLit{}; },
          [](AST::BooleanLit &) -> Lit::LiteralKind { return BooleanLit{}; },
          [](AST::FloatLit &) -> Lit::LiteralKind { return FloatLit{}; },
          [](AST::StrLit &) -> Lit::LiteralKind { return StrLit{}; }},
      lit.kind);

  return result;
}

Pat MrBuilder::make_pat(AST::Pat &pat) {
  return std::visit(
      overloaded{[&](AST::IdentPat &val) -> Pat {
                   auto new_pattern = val.pattern.has_value()
                                          ? Opt<U<Pat>>(MU<Pat>(
                                                make_pat(*val.pattern.value())))
                                          : std::nullopt;

                   return Pat(IdentPat(val.binding, val.identifier.symbol,
                                       std::move(new_pattern)));
                 },
                 [&](AST::LitPat &val) -> Pat {
                   return Pat(LitPat{make_expr(*val.expr)});
                 }},
      pat.kind);
}

Path MrBuilder::make_path(AST::Path &path) {
  return Path{
      .segments = path.segments,
  };
}

Stmt MrBuilder::lower_local(AST::Local &local) {
  Pat pat = make_pat(*local.pat);

  return std::visit(
      overloaded{[&](AST::InitLocal &init) -> Stmt {
                   return Stmt(LetStmt{.pattern = std::move(pat),
                                       .initializer = make_expr(*init.expr),
                                       .else_block = std::nullopt});
                 },
                 [&](AST::InitElseLocal &init) -> Stmt {
                   return Stmt(LetStmt{.pattern = std::move(pat),
                                       .initializer = make_expr(*init.expr),
                                       .else_block = make_block(*init.block)});
                 },
                 [&](AST::DeclLocal &decl) -> Stmt {
                   return Stmt(LetStmt{.pattern = std::move(pat),
                                       .initializer = std::nullopt,
                                       .else_block = std::nullopt});
                 }},
      local.kind);
}

Stmt MrBuilder::lower_item(AST::Item &item) {
  return std::visit(
      overloaded{[&](AST::FnItem &val) -> Stmt {
        auto fn = make_fn(*val.fn);

        PathExpr path_expr{Path{{item.ident.symbol}}};

        std::vector<Id> stmts;

        Pat fn_pat{IdentPat{AST::BindingMode::make(), // Default binding mode
                            item.ident.symbol, std::nullopt}};

        return Stmt(LetStmt{std::move(fn_pat), fn, std::nullopt});
      }},
      item.kind);
}

Mr MrBuilder::build(AST::Ast &ast) {
  for (auto &item : ast.items) {
    auto lowered = lower_item(item);
    mr.insert_stmt(std::move(lowered));
  }

  return std::move(mr);
}

Id MrBuilder::make_block(AST::Block &block) {
  Block result;

  for (auto &stmt : block.statements) {
    result.statements.push_back(make_stmt(stmt));
  }

  return mr.insert_block(result);
}

Id MrBuilder::make_fn(AST::Fn &fn) {
  Fn result;

  if (fn.body.has_value()) {
    result.body = make_block(*fn.body.value());
  }

  for (auto &param : fn.params) {
    result.params.push_back(make_param(param));
  }

  return mr.insert_fn(result);
}

Id MrBuilder::make_expr(AST::Expr &expr) {
  Expr result;
  result.type = std::move(resolve_type(expr.id, *type_context));

  result.kind = std::visit(
      overloaded{
          [&](AST::LitExpr &val) -> Expr::ExprKind {
            return LitExpr{make_lit(*val.lit)};
          },
          [&](AST::ExprExpr &val) -> Expr::ExprKind {
            return ExprExpr{make_expr(*val.expr)};
          },
          [&](AST::LetExpr &val) -> Expr::ExprKind {
            return LetExpr{make_pat(*val.pattern), make_expr(*val.expr)};
          },
          [&](AST::BlockExpr &val) -> Expr::ExprKind {
            return BlockExpr{make_block(*val.block)};
          },
          [&](AST::IfExpr &val) -> Expr::ExprKind {
            auto cond_id = make_expr(*val.expr);
            auto block_id = make_block(*val.block);
            Opt<Id> else_expr =
                val.elseExpr
                    ? std::make_optional(make_expr(*val.elseExpr.value()))
                    : std::nullopt;
            return IfExpr{cond_id, block_id, else_expr};
          },
          [&](AST::LoopExpr &val) -> Expr::ExprKind {
            return LoopExpr{make_block(*val.block)};
          },
          [&](AST::WhileExpr &val) -> Expr::ExprKind {
            return WhileExpr{make_expr(*val.expr), make_block(*val.block)};
          },
          [&](AST::CallExpr &val) -> Expr::ExprKind {
            std::vector<Id> param_ids;
            for (auto &param : val.params) {
              param_ids.push_back(make_expr(*param));
            }
            return CallExpr{make_expr(*val.expr), std::move(param_ids)};
          },
          [&](AST::PathExpr &val) -> Expr::ExprKind {
            return PathExpr{make_path(*val.path)};
          },
          [&](AST::BinaryExpr &val) -> Expr::ExprKind {
            return BinaryExpr{val.op, make_expr(*val.first),
                              make_expr(*val.second)};
          },
          [&](AST::AssignExpr &val) -> Expr::ExprKind {
            return AssignExpr{make_expr(*val.first), make_expr(*val.second)};
          },
          [&](AST::AssignOpExpr &val) -> Expr::ExprKind {
            return AssignOpExpr{val.op, make_expr(*val.first),
                                make_expr(*val.second)};
          },
          [&](AST::BreakExpr &val) -> Expr::ExprKind {
            return BreakExpr{
                val.expr ? std::make_optional(make_expr(*val.expr.value()))
                         : std::nullopt};
          },
          [&](AST::ReturnExpr &val) -> Expr::ExprKind {
            return ReturnExpr{
                val.expr ? std::make_optional(make_expr(*val.expr.value()))
                         : std::nullopt};
          },
          [&](AST::ContinueExpr &) -> Expr::ExprKind {
            return ContinueExpr{};
          }},
      expr.kind);

  return mr.insert_expr(std::move(result));
}

Id MrBuilder::make_param(AST::Param &param) {
  Param p;
  return mr.insert_param(
      Param{.pat = make_pat(*param.pat),
            .type = std::move(resolve_type(param.id, *type_context))});
}

Id MrBuilder::make_stmt(AST::Stmt &stmt) {
  return std::visit(
      overloaded{[&](AST::ExprStmt &val) -> Id {
                   auto expr_id = make_expr(*val.expr);
                   return mr.insert_stmt(Stmt(ExprStmt{expr_id}));
                 },
                 [&](AST::LetStmt &val) -> Id {
                   auto lowered = lower_local(*val.local);
                   return mr.insert_stmt(std::move(lowered));
                 },
                 [&](AST::EmptyStmt &val) -> Id {
                   auto expr_id = mr.insert_expr(Expr(UnitExpr()));
                   return mr.insert_stmt(Stmt(ExprStmt{.expr = expr_id}));
                 },
                 [&](AST::ItemStmt &val) -> Id {
                   auto lowered = lower_item(*val.item);
                   return mr.insert_stmt(std::move(lowered));
                 },
                 [&](AST::SemiStmt &val) -> Id {
                   auto expr_id = make_expr(*val.expr);
                   return mr.insert_stmt(Stmt(ExprStmt{expr_id}));
                 }},
      stmt.kind);
}

} // namespace MRC::MR
