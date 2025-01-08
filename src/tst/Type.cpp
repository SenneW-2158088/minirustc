#include "typechecking/Type.h"

namespace MRC::TS {

FunctionType::FunctionType() = default;

FunctionType::FunctionType(std::vector<U<CheckType>> params, U<CheckType> return_type) {
 this->params = std::move(params);
 this->return_type = std::move(return_type);
}

FunctionType::FunctionType(const FunctionType &other) {
 params.reserve(other.params.size());
 for (const auto &param : other.params) {
   params.push_back(MU<CheckType>(*param));
 }
 return_type = MU<CheckType>(*other.return_type);
}

FunctionType& FunctionType::operator=(const FunctionType& other) {
   if (this != &other) {
       params.clear();
       params.reserve(other.params.size());
       for (const auto& param : other.params) {
           params.push_back(MU<CheckType>(*param));
       }
       return_type = MU<CheckType>(*other.return_type);
   }
   return *this;
}

Type::Type(TypeKind kind) : kind(kind) {}

Type Type::makeInt(bool is_signed, uint8_t bits) {
 return Type(IntType{is_signed, bits});
}

Type Type::makeFloat(uint8_t bits) { 
 return Type(FloatType{bits}); 
}

Type Type::makeVoid() { 
 return Type(VoidType{}); 
}

Type Type::makeBool() { 
 return Type(BoolType{}); 
}

Type Type::makeUnset() { 
 return Type(UnsetType{}); 
}

Type Type::makeString() { 
 return Type(StringType{}); 
}

Type Type::makeFunction(std::vector<U<CheckType>> params, U<CheckType> return_type) {
 return Type(FunctionType{std::move(params), std::move(return_type)});
}

bool Type::unionize(Type &other) {
 return std::visit(
     overloaded{
         [&](IntType &left, IntType &right) -> bool {
           if (left.bits >= right.bits && left.is_signed == right.is_signed) {
             right = left;
             return true;
           } else if (left.bits <= right.bits && left.is_signed == right.is_signed) {
             left = right;
             return true;
           }
           return false;
         },
         [&](FloatType &left, FloatType &right) -> bool {
           if (left.bits >= right.bits) {
             right = left;
             return true;
           }
           left = right;
           return true;
         },
         [&](FunctionType &left, FunctionType &right) -> bool {
           if (left.params.size() != right.params.size())
             return false;
           if (!left.return_type->equals(*right.return_type))
             return false;
           for (int i = 0; i < left.params.size(); ++i) {
             if (!left.params[i]->unionize(*right.params[i]))
               return false;
           }
           return true;
         },
         [&](auto &left, UnsetType &right) -> bool { return true; },
         [&](auto &left, auto &right) -> bool {
           return typeid(left) == typeid(right);
         }},
     this->kind, other.kind);
}

CheckType::CheckType(TypeKind kind) : kind(kind) {}

CheckType CheckType::makeVar(Type type) {
 return CheckType(VarType{.type = type});
}

CheckType CheckType::makeConcrete(Type type) {
 return CheckType(ConcreteType{.type = type});
}

bool CheckType::equals(CheckType &other) {
 return std::visit(
     overloaded{
         [&](VarType &val, ConcreteType &other) -> bool {
           return val.type.equals(other.type);
         },
         [&](VarType &val, VarType &other) -> bool {
           return val.type.equals(other.type);
         },
         [&](ConcreteType &val, ConcreteType &other) -> bool {
           return val.type.equals(other.type);
         },
         [&](auto &val, auto &other) -> bool {
           return val.type.equals(other.type);
         }},
     this->kind, other.kind);
}

bool CheckType::is_variant() const {
 return std::holds_alternative<VarType>(kind);
}

bool CheckType::is_concrete() const {
 return std::holds_alternative<ConcreteType>(kind);
}

bool CheckType::unionize(CheckType &other) {
 return std::visit(
     overloaded{
         [&](VarType &val, VarType &other_val) -> bool {
           if (val.type.unionize(other_val.type)) {
             other.kind = VarType(val.type);
             return true;
           }
           return false;
         },
         [&](ConcreteType &val, VarType &other_val) -> bool {
           if (val.type.unionize(other_val.type)) {
             other.kind = ConcreteType(val.type);
             return true;
           }
           return false;
         },
         [&](ConcreteType &val, ConcreteType &other_val) -> bool {
           return val.type.equals(other_val.type);
         },
         [&](auto &val, auto &other_val) -> bool { return false; }},
     this->kind, other.kind);
}

std::string CheckType::to_string() {
 return std::visit(
     overloaded{
         [&](VarType &val) -> std::string {
           return "varType:" + val.type.to_string();
         },
         [&](ConcreteType &val) -> std::string {
           return "ConcreteType:" + val.type.to_string();
         }},
     this->kind);
}
}

bool MRC::TST::Type::equals(const Type& other) const {
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
