#pragma once

#include "ast/prelude.h"
#include "ast/visitor/Visitor.h"
#include "typechecking/TypeChecker.h"

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


// Represents a runtime value 
struct Value {
  union Stored {
        
  };

  Stored stored;

  Value() = default;
};

struct Variable {
  Value value;  
  bool is_mutable;

  Variable() = default;
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
    P<Environment> parent;
    std::vector<P<Environment>> children;

    std::map<std::string, P<Variable>> variables;

    // For controlling how the program executes
    Opt<Flow> flow;
    ControlFlow control;

    // Evaluate an expression
    Value evalute_item(AST::Item &item);
    Value evaluate_expr(AST::Expr& expr);
    Value evaluate_stmt(AST::Expr& expr);
    Value evaluate_local(AST::Local& expr);
    Value evaluate_block(AST::Block& expr);
};

// Interpreter visitor
class Interpreter : public AST::Visitor {
private:
  P<TS::TypeContext> type_context;
public:
  Interpreter(P<TS::TypeContext> context);
  void visit_item(AST::Item &item) override;
  void visit_stmt(AST::Stmt &stmt) override;
  void visit_expr(AST::Expr &expr) override;
  void visit_local(AST::Local &local) override;

  // Start the interpreter
  void interp(std::string &entry);
};

} // namespace MRC::INTERP
