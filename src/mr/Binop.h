
#pragma once
#include <variant>
#include <string>

#include "util/util.h"

namespace MRC::MR{
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
