#pragma once

#include <stack>
#include <map>

#include "ast/visitor/Visitor.h"
#include "ast/prelude.h"
#include "tst/prelude.h"

namespace MRC::AST {

class TstVisitor : public Visitor {
private:
    TST::Tst tst;
    std::stack<TST::Index> expr_stack;
    std::stack<TST::Index> stmt_stack;
    std::stack<TST::Index> block_stack;

    // Track variable bindings and their types
    std::map<std::string, TST::Index> symbol_table;

    // Type creation helpers
    TST::Index create_type_from_ast_type(const Type& ast_type) {
        return std::visit(overloaded{
            [&](const PathType& path_type) -> TST::Index {
                // Convert path type (like i32, f64) to TST type
                const auto& segments = path_type.path->segments;
                if (segments.empty()) {
                    throw std::runtime_error("Empty path type");
                }

                // Handle built-in types
                if (segments[0] == "i32") {
                    return tst.insert_type(TST::Type::makeInt(true, 32));
                } else if (segments[0] == "i64") {
                    return tst.insert_type(TST::Type::makeInt(true, 64));
                } else if (segments[0] == "f64") {
                    return tst.insert_type(TST::Type::makeFloat(64));
                }
                // Add more types as needed

                throw std::runtime_error("Unknown type: " + segments[0]);
            },
            [&](const InferType&) -> TST::Index {
                // For now, default to i32 for inference
                return tst.insert_type(TST::Type::makeInt(true, 32));
            }
        }, ast_type.kind);
    }

public:
    TST::Tst take_tst() { return std::move(tst); }

    void visit_item(Item& item) override {
        std::visit(overloaded{
            [&](FnItem& fn_item) {
                // Process function item
                visit_fn(*fn_item.fn);

                // Create function parameters
                std::vector<TST::Param> params;
                for (const auto& param : fn_item.fn->params) {
                    TST::Index param_type = create_type_from_ast_type(*param.type);
                    TST::Index param_pattern = process_pattern(*param.pat);
                    params.push_back(TST::Param{param_pattern, param_type});
                }

                TST::Index return_type;
                if (fn_item.fn->type) {
                    return_type = create_type_from_ast_type(*fn_item.fn->type.value());
                }

                std::vector<TST::Index> fn_locals;
                Opt<TST::Index> body;
                if (fn_item.fn->body) {
                    body = process_block(*fn_item.fn->body.value());
                }

                // Create function
                TST::Function function{
                    std::move(params),
                    return_type,
                    body,
                    std::move(fn_locals),
                };

                tst.insert_function(std::move(function));
            }
        }, item.kind);
    }

    void visit_expr(Expr& expr) override {
        TST::Index expr_idx = std::visit(overloaded{
            [&](LitExpr& lit_expr) -> TST::Index {
                visit_lit(*lit_expr.lit);
                TST::Index type_idx = create_type_from_literal(*lit_expr.lit);
                return tst.insert_expr(TST::Expr::makeLit(
                    expr_stack.top(),  // literal value index
                    type_idx
                ));
            },
            [&](BlockExpr& block_expr) -> TST::Index {
                TST::Index block_idx = process_block(*block_expr.block);
                TST::Index type_idx = get_block_type(*block_expr.block);
                return tst.insert_expr(TST::Expr::makeBlock(block_idx, type_idx));
            },
            [&](LoopExpr& loop_expr) -> TST::Index {
                TST::Index block_idx = process_block(*loop_expr.block);
                TST::Index type_idx = create_unit_type();
                return tst.insert_expr(TST::Expr::makeLoop(block_idx, type_idx));
            },
            [&](WhileExpr& while_expr) -> TST::Index {
                return desugar_while(*while_expr.expr, *while_expr.block);
            },
            // Add other expression handlers...
            [&](auto&) -> TST::Index {
                throw std::runtime_error("Unhandled expression type");
            }
        }, expr.kind);

        expr_stack.push(expr_idx);
    }

    void visit_stmt(Stmt& stmt) override {
        TST::Index stmt_idx = std::visit(overloaded{
            [&](ExprStmt& expr_stmt) -> TST::Index {
                visit_expr(*expr_stmt.expr);
                TST::Index expr_idx = expr_stack.top();
                expr_stack.pop();

                return tst.insert_stmt(TST::Stmt::makeExpr(expr_idx));
            },
            [&](LetStmt& let_stmt) -> TST::Index {
                return process_let_stmt(*let_stmt.local);
            },
            [&](ItemStmt& item_stmt) -> TST::Index {
                visit_item(*item_stmt.item);
                // Item statements don't create a value in TST
                return tst.insert_stmt(TST::Stmt::makeItem(expr_stack.top()));
            }
        }, stmt.kind);

        stmt_stack.push(stmt_idx);
    }

private:
    TST::Index process_pattern(Pat& pat) {
        // Process pattern and return its index
        return 0; // Placeholder
    }

    TST::Index process_block(Block& block) {
        std::vector<TST::Index> stmts;
        for (auto& stmt : block.statements) {
            visit_stmt(stmt);
            stmts.push_back(stmt_stack.top());
            stmt_stack.pop();
        }

        return tst.insert_block(TST::Block{
            std::move(stmts),
            std::nullopt,  // expr
            create_unit_type()  // block type
        });
    }

    TST::Index process_let_stmt(Local& local) {
        TST::Index pattern = process_pattern(*local.pat);
        TST::Index type;

        if (local.type) {
            type = create_type_from_ast_type(*local.type.value());
        } else {
            type = create_unit_type(); // Default type if not specified
        }

        Opt<TST::Index> init;
        Opt<TST::Index> else_block;

        std::visit(overloaded{
            [&](const InitLocal& init_local) {
                visit_expr(*init_local.expr);
                init = expr_stack.top();
                expr_stack.pop();
            },
            [&](const InitElseLocal& init_else) {
                visit_expr(*init_else.expr);
                init = expr_stack.top();
                expr_stack.pop();

                visit_block(*init_else.block);
                else_block = block_stack.top();
                block_stack.pop();
            },
            [](const auto&) {}
        }, local.kind);

        TST::Local tst_local{pattern, type, init, else_block};
        TST::Index local_idx = tst.insert_local(std::move(tst_local));

        return tst.insert_stmt(TST::Stmt::makeLet(local_idx));
    }

    TST::Index desugar_while(Expr& condition, Block& body) {
        // Create a block for the condition check
        visit_expr(condition);
        TST::Index cond_idx = expr_stack.top();
        expr_stack.pop();

        // Process the body
        TST::Index body_idx = process_block(body);

        // Create the loop body that includes condition check
        std::vector<TST::Index> loop_stmts;

        // Add condition check and break
        // ... build condition check and break statements ...

        TST::Index type_idx = create_unit_type();

        // Create the final loop block
        TST::Index loop_block = tst.insert_block(TST::Block{
            std::move(loop_stmts),
            std::nullopt,
            type_idx
        });

        return tst.insert_expr(TST::Expr::makeLoop(loop_block, type_idx));
    }
};

struct TstBuilder {
    TST::Tst build(Ast& ast) {
        TstVisitor visitor;

        for (auto& item : ast.items) {
            visitor.visit_item(item);
        }

        for (auto& stmt : ast.statements) {
            visitor.visit_stmt(stmt);
        }

        return visitor.take_tst();
    }
};

} // namespace MRC::AST
