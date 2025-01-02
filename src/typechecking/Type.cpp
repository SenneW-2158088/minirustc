#include "Type.h"
#include <format>

std::string MRC::TS::Type::to_string() const {
    return std::visit(overloaded{
        [](const IntType& a) -> std::string {
            return std::format("int ({},{})", a.bits, a.is_signed);
        },
        [](const FloatType& a) -> std::string {
            return std::format("float ({})", a.bits);
        },
        [](const BoolType&) -> std::string {
            return "bool";
        },
        [](const StringType&) -> std::string {
            return "string";
        },
        [](const FunctionType& a) -> std::string {
            return "function";
        },
        [](const VoidType& a) -> std::string {
            return "void";
        },
        [](const UnsetType& a) -> std::string {
            return "unset";
        },
    }, kind);
}

bool MRC::TS::Type::equals(const Type& other) const {
    if (kind.index() != other.kind.index()) {
        return false;
    }

    return std::visit(overloaded{
        [](const IntType& a, const IntType& b) -> bool {
            return a.is_signed == b.is_signed && a.bits == b.bits;
        },
        [](const FloatType& a, const FloatType& b) -> bool {
            return a.bits == b.bits;
        },
        [](const BoolType&, const BoolType&) -> bool {
            return true;
        },
        [](const StringType&, const StringType&) -> bool {
            return true;
        },
        [](const FunctionType& a, const FunctionType& b) -> bool {
            if (a.params.size() != b.params.size()) {
                return false;
            }

            for (size_t i = 0; i < a.params.size(); i++) {
                if (!a.params[i]->equals(*b.params[i])) {
                    return false;
                }
            }

            return a.return_type->equals(*b.return_type);
        },
        [](const auto&, const auto&) -> bool {
            return false;
        }
    }, kind, other.kind);
}
