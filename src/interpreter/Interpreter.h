#pragma once

#include "ast/prelude.h"
#include "mr/Mr.h"
#include "mr/prelude.h"
#include "mr/visitor/Visitor.h"
#include "typechecking/TypeChecker.h"
#include "interpreter/Type.h"

namespace MRC::INTERP {

class RuntimeError : public std::runtime_error {
public:
  explicit RuntimeError(const std::string &msg) : std::runtime_error(msg) {}
};

class ImmutableError : public RuntimeError {
public:
  explicit ImmutableError(const std::string &name)
      : RuntimeError("Cannot modify immutable variable: " + name) {}
};

struct Flow {
    void entry(); // begin flow
    void exit(); // exit flow
    void next(); // continue flow
    void stop(); // break flow
};

struct ControlFlow {
  enum class Type {
    None,
    Return,
    Break,
    Continue
  };

  Type type = Type::None;
  Opt<Value> value;
};

struct Block {
  
};

struct Environment {
    MR::Mr &mr;
    P<MR::SymbolTable> symbol_table;

    Environment *parent;
    std::vector<P<Environment>> children;

    std::map<std::string, P<Variable>> variables;

    Opt<Flow> flow;
    ControlFlow control;

    // Evaluate an expression
    Value evaluate_fn(MR::Id fn);
    Value evaluate_expr(MR::Id & expr);
    Value evaluate_stmt(MR::Id & stmt);
    Value evaluate_local(MR::Id & local);
    Value evaluate_block(MR::Id & block);

    void declare_variable(const std::string& name, bool is_mutable = false);
    void set_variable(const std::string& name, const Value& value);
    Value get_variable(const std::string& name);
    bool has_variable(const std::string& name);
    Variable* lookup_variable(const std::string& name);
    Variable* lookup_variable_mut(const std::string& name);
    void print(int depth = 0) const;

    Environment(MR::Mr &mr) : mr(mr) {}
    Environment(Environment *parent, MR::Mr &mr, P<MR::SymbolTable> symbol_table, Opt<Flow> flow = std::nullopt)
      : parent(parent), mr(mr), symbol_table(symbol_table), flow(flow), variables{} {}

};

// Interpreter visitor
class Interpreter {
private:
  P<TS::TypeContext> type_context;
  P<Environment> environment;
  MR::Mr &mr;
public:
  Interpreter(P<TS::TypeContext> context, MR::Mr& mr);
  
  // Start the interpreter
  void interp(std::string &entry);
};

} // namespace MRC::INTERP
