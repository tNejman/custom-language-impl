#pragma once

#include <functional>
#include <memory>
#include <stack>
#include <string>
#include <string_view>
#include <variant>

#include "Interpreter/BuiltinFunctions.hpp"
#include "Interpreter/CallStack.h"
#include "Interpreter/RuntimeValue.h"
#include "Interpreter/Variable.h"
#include "Parser/Node.h"
#include "Parser/ParameterDecl.hpp"
#include "Parser/Visitor.h"

using FunctionSymbol = std::variant<std::reference_wrapper<const FunctionDefNode>, BuiltinFunction>;

class InterpreterTest;

class Interpreter : public Visitor {
  friend class InterpreterTest;

 private:
  struct CallContextGuard;
  struct AccumulatorGuard;
  class Environment {
    friend struct CallContextGuard;

   public:
    enum class ControlFlow { BREAK, CONTINUE, NONE, RETURN };

   private:
    std::vector<FunctionSymbol> functions_;
    CallStack call_stack_;

    ControlFlow loop_control_type_;

    std::optional<std::reference_wrapper<const FunctionSymbol>> getBuiltinFunctionByIdentifier(
        const std::string identifier ) const noexcept;
    std::optional<std::reference_wrapper<const FunctionSymbol>> getFunctionBySignature(
        const FunctionDefNode& node ) const noexcept;
    std::optional<std::reference_wrapper<const FunctionSymbol>> getFunctionBySignature(
        const BuiltinFunction& func ) const noexcept;
    std::optional<std::reference_wrapper<const FunctionSymbol>> getFunctionBySignature(
        const std::string_view identifier, const Type& return_type,
        const std::vector<ParameterDecl>& parameters ) const noexcept;

   public:
    std::vector<std::reference_wrapper<const FunctionSymbol>> getFunctionByIdentifier(
        const std::string_view identifier ) const noexcept;
    std::optional<std::reference_wrapper<const FunctionSymbol>> getFunctionBySignature(
        const std::string_view identifier, const std::vector<RuntimeValue>& call_args ) const noexcept;

    bool tryAddUserFunction( const FunctionDefNode& node );
    bool tryAddBuiltinFunction( BuiltinFunction function );

    bool tryAddVarOrConst( Variable variable );
    std::optional<std::reference_wrapper<const Variable>> getVarByName(
        const std::string_view identifier ) const noexcept;
    std::optional<std::reference_wrapper<Variable>> getVarByName( const std::string_view identifier ) noexcept;
    std::optional<std::reference_wrapper<const Variable>> getVarByNameThisScopeOnly(
        const std::string_view identifier ) const noexcept;
    std::optional<std::reference_wrapper<Variable>> getVarByNameThisScopeOnly(
        const std::string_view identifier ) noexcept;

    bool isStateInWhileLoop() const noexcept;
    bool isStateInFunctionBody() const noexcept;
    // const Type& getCurrentFunctionReturnType() const;

    bool matchFunctionSignature( const std::vector<ParameterDecl>& params,
                                 const std::vector<RuntimeValue>& call_args ) const noexcept;

    ControlFlow getFlowControlType() const noexcept;
    void setFlowControlType( ControlFlow ) noexcept;

  } environment_;
  friend struct AccumulatorGuard;
  std::unique_ptr<const ProgramNode> program_;
  std::stack<RuntimeValue> accumulator_;

  RuntimeValue getRecentValFromAcc();
  void putValInAcc( RuntimeValue acc_val ) noexcept;

  Variable buildVarFromParam( RuntimeValue& runtime_val, const ParameterDecl& param ) noexcept;

  void handleStatementList( const std::vector<std::unique_ptr<INode>>&, CallContext::ContextType context_type );

  struct CallContextGuard {
   private:
    Environment& env_;

   public:
    CallContextGuard( Environment& env, CallContext::ContextType context_type );
    CallContextGuard( Environment& env, const FunctionDefNode& func_def );
    ~CallContextGuard();
  };

  struct AccumulatorGuard {
   private:
    std::stack<RuntimeValue>& acc_;
    size_t org_size_;

   public:
    AccumulatorGuard( std::stack<RuntimeValue>& acc ) noexcept;
    ~AccumulatorGuard() noexcept;
  };

 public:
  explicit Interpreter( std::unique_ptr<const ProgramNode> program );

  void execute();

  void visit( const FunctionDefNode& node ) override;
  void visit( const VarOrConstDeclNode& node ) override;
  void visit( const IfStatementNode& node ) override;
  void visit( const WhileStatementNode& node ) override;
  void visit( const ControlFlowNode& node ) override;
  void visit( const ReturnNode& node ) override;
  void visit( const AssignmentExprNode& node ) override;
  void visit( const BinaryExprNode& node ) override;
  void visit( const UnaryExprNode& node ) override;
  void visit( const CastExprNode& node ) override;
  void visit( const ArrayIdentifierOpNode& node ) override;
  void visit( const FunctionCallNode& node ) override;
  void visit( const ArrayLiteralNode& node ) override;
  void visit( const LiteralExprNode& node ) override;
  void visit( const PrimaryIdentifierNode& node ) override;
  void visit( const ProgramNode& node ) override;
};