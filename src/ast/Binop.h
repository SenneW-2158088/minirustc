#pragma once
#include <variant>
#include <string>

#include "util/util.h"


namespace MRC::AST {

        struct Add {};
        struct Sub {};
        struct Mul {};
        struct Div {};
        struct And {};
        struct Or {};
        struct Eq {};
        struct Lt {};
        struct Le {};
        struct Ne {};
        struct Ge {};
        struct Gt {};
    struct BinOp {
        using Kind = std::variant<Add, Sub, Mul, Div,
                                And, Or,
                                Eq, Lt, Le, Ne, Ge, Gt>;
        Kind kind;

        BinOp() = default;
        explicit BinOp(Kind kind) : kind(std::move(kind)) {}

        static BinOp makeAdd() { return BinOp(Add{}); }
        static BinOp makeSub() { return BinOp(Sub{}); }
        static BinOp makeMul() { return BinOp(Mul{}); }
        static BinOp makeDiv() { return BinOp(Div{}); }
        static BinOp makeAnd() { return BinOp(And{}); }
        static BinOp makeOr() { return BinOp(Or{}); }
        static BinOp makeEq() { return BinOp(Eq{}); }
        static BinOp makeLt() { return BinOp(Lt{}); }
        static BinOp makeLe() { return BinOp(Le{}); }
        static BinOp makeNe() { return BinOp(Ne{}); }
        static BinOp makeGe() { return BinOp(Ge{}); }
        static BinOp makeGt() { return BinOp(Gt{}); }

        std::string to_string() const {
            return std::visit(overloaded{
                [](const Add&) -> std::string { return "+"; },
                [](const Sub&) -> std::string { return "-"; },
                [](const Mul&) -> std::string { return "*"; },
                [](const Div&) -> std::string { return "/"; },
                [](const And&) -> std::string { return "&&"; },
                [](const Or&) -> std::string { return "||"; },
                [](const Eq&) -> std::string { return "=="; },
                [](const Lt&) -> std::string { return "<"; },
                [](const Le&) -> std::string { return "<="; },
                [](const Ne&) -> std::string { return "!="; },
                [](const Ge&) -> std::string { return ">="; },
                [](const Gt&) -> std::string { return ">"; }
            }, kind);
        }
    };
}
