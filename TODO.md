# Basic

- [x] i32 + bools
- [x] Integer literals
- [x] Bool literals
- [x] let, let mut statements

# DONE

* Create and test parsing of all basic test cases
* Complete the russt specific type declarations
* Start type checker, data structures + basic Typed Syntax Tree (TST)

# TODO

* Type inference with let statement (let a: i32 = 30)
* Type inference with expression statement (a = b + c)


    | IF expr block_expr ELSE block_expr {
        auto b_expr = MU<Expr>(Expr::makeBlock(ast->getId(), std::move($3)));
        $$ = MU<Expr>(Expr::makeIfElse(ast->getId(), std::move($2), std::move($3), std::move(b_expr)));
    }
    | IF expr block_expr ELSE if_expr {
        $$ = MU<Expr>(Expr::makeIfElse(ast->getId(), std::move($2), std::move($3), std::move($5)));
    }
