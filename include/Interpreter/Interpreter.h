#pragma once

#include <functional>
#include <memory>
#include <stack>
#include <string_view>
#include <variant>

#include "Interpreter/BuiltinFunctions.hpp"
#include "Interpreter/CallStack.h"
#include "Interpreter/RuntimeValue.hpp"
#include "Interpreter/Variable.h"
#include "Parser/Node.h"
#include "Parser/ParameterDecl.hpp"
#include "Parser/Visitor.h"

using FunctionSymbol = std::variant<std::reference_wrapper<const FunctionDefNode>, BuiltinFunction>;

class Interpreter : public Visitor {
 private:
  class Environment {
   public:
    enum class ControlFlow { BREAK, CONTINUE, NONE, RETURN };

   private:
    std::vector<FunctionSymbol> functions_;
    CallStack call_stack_;
    std::stack<RuntimeValue> accumulator_;

    ControlFlow loop_control_type_;

    std::optional<std::reference_wrapper<const FunctionSymbol>> getFunctionByIdentifier(
        const std::string_view identifier ) const noexcept;
    std::optional<std::reference_wrapper<const FunctionSymbol>> getFunctionBySignature(
        const FunctionDefNode& node ) const noexcept;
    std::optional<std::reference_wrapper<const FunctionSymbol>> getFunctionBySignature(
        const BuiltinFunction& func ) const noexcept;
    std::optional<std::reference_wrapper<const FunctionSymbol>> getFunctionBySignature(
        const std::string_view identifier, const Type& return_type,
        const std::vector<ParameterDecl>& parameters ) const noexcept;

   public:
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

    RuntimeValue getRecentValFromAcc();
    void putValInAcc( RuntimeValue acc_val ) noexcept;
    size_t getAccSize() const noexcept;

    void addCallContext( CallContext::ContextType context_type );
    void addCallContext( const FunctionDefNode& func_def );
    void popLastCallContext();

    bool isStateInWhileLoop() const noexcept;
    bool isStateInFunctionBody() const noexcept;
    const Type& getCurrentFunctionReturnType() const;

    bool matchFunctionSignature( const std::vector<ParameterDecl>& params,
                                 const std::vector<RuntimeValue>& call_args ) const noexcept;
    Variable buildVarFromParam( RuntimeValue& runtime_val, const ParameterDecl& param ) noexcept;

    ControlFlow getFlowControlType() const noexcept;
    void setFlowControlType( ControlFlow ) noexcept;

  } environment_;
  std::unique_ptr<const ProgramNode> program_;

  struct CallContextGuard {
    Environment& env_;
    CallContextGuard( Environment& env, CallContext::ContextType context_type ) : env_( env ) {
      env.addCallContext( context_type );
    }
    CallContextGuard( Environment& env, const FunctionDefNode& func_def ) : env_( env ) {
      env.addCallContext( func_def );
    }
    ~CallContextGuard() {
      env_.popLastCallContext();
    }
  };

  void handleStatementList( const std::vector<std::unique_ptr<INode>>&, CallContext::ContextType context_type );

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
  void visit( const FunctionCallNode& node ) override;
  void visit( const ArrayLiteralNode& node ) override;
  void visit( const LiteralExprNode& node ) override;
  void visit( const PrimaryIdentifierNode& node ) override;
  void visit( const ProgramNode& node ) override;
};