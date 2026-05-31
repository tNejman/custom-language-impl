#pragma once

#include <functional>
#include <memory>
#include <stack>
#include <string_view>
#include <type_traits>
#include <variant>

#include "Interpreter/BuiltinFunctions.hpp"
#include "Interpreter/CallStack.h"
#include "Interpreter/Variable.h"
#include "Parser/Node.h"
#include "Parser/ParameterDecl.hpp"
#include "Parser/Value.hpp"
#include "Parser/Visitor.h"

using FunctionSymbol = std::variant<const FunctionDefNode*, BuiltinFunction>;

using RValue = Value;
using LValue = std::reference_wrapper<Variable>;
using AccumulatorVal = std::variant<LValue, RValue>;

class Interpreter : public Visitor {
 private:
  class Environment {
   private:
    std::unique_ptr<const ProgramNode> program_;
    std::vector<FunctionSymbol> functions_;
    CallStack call_stack_;
    std::stack<AccumulatorVal> accumulator_;

    const FunctionSymbol* getFunctionByIdentifier( const std::string_view identifier ) const noexcept;
    const FunctionSymbol* getFunctionBySignature( const FunctionDefNode* node ) const noexcept;
    const FunctionSymbol* getFunctionBySignature( const BuiltinFunction& func ) const noexcept;
    const FunctionSymbol* getFunctionBySignature( const std::string_view identifier, const Type& return_type,
                                                  const std::vector<ParameterDecl>& parameters ) const noexcept;

   public:
    Environment( std::unique_ptr<const ProgramNode> program );

    bool tryAddUserFunction( const FunctionDefNode* node );
    bool tryAddBuiltinFunction( BuiltinFunction function );

    bool tryAddVarOrConst( Variable variable );
    const Variable* getVarOrConstByNameCurScope( const std::string_view identifier ) const noexcept;
    Variable* getVarOrConstByNameCurScope( const std::string_view identifier ) noexcept;
    Variable* getVarOrConstByNameGlobalScope( const std::string_view identifier ) noexcept;

    bool isAssignableTo( const AccumulatorVal& val ) const noexcept;

    AccumulatorVal getRecentValFromAcc();
    void putValInAcc( Value val ) noexcept;
    void putValInAcc( std::reference_wrapper<Variable> var ) noexcept;
    size_t getAccSize() const noexcept;

    void addCallContext( CallContext::ContextType context_type );
    void addCallContext( const FunctionDefNode* func_def );
    void popLastCallContext();

    bool isStateInWhileLoop() const noexcept;
    bool isStateInFunctionBody() const noexcept;
    const Type& getCurrentFunctionReturnType() const;

  } environment_;

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