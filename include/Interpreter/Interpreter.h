#pragma once

#include <functional>
#include <memory>
#include <stack>
#include <type_traits>
#include <variant>

#include "Interpreter/BuiltinFunctions.hpp"
#include "Interpreter/Variable.h"
#include "Parser/Node.h"
#include "Parser/ParameterDecl.hpp"
#include "Parser/Value.hpp"
#include "Parser/Visitor.h"

struct CallContext {
 public:
  enum class ContextType { TOP_LEVEL, IF_BLOCK, FUNCTION_CALL, WHILE_BLOCK };

 private:
  ContextType type_;
  std::vector<Variable> variables_;

  struct TraceWrapper {};

 public:
  void addVariable( Variable variable ) noexcept;
  const std::vector<Variable>& getVariables() const noexcept;
  std::vector<Variable>& getVariables() noexcept;
};

// class StackGuard {
//  private:
//   std::stack<Value>& target_stack_;

//  public:
//   StackGuard( std::stack<Value>& stack, const Value& value ) : target_stack_( stack ) {
//     target_stack_.push( value );
//   }
// };

using FunctionSymbol = std::variant<const FunctionDefNode*, BuiltinFunction>;

using AccumulatorVal = std::variant<Value, const FunctionDefNode*, const Variable*>;

class Interpreter : public Visitor {
 private:
  class Environment {
   private:
    std::unique_ptr<const ProgramNode> program_;
    std::vector<FunctionSymbol> functions_;
    std::stack<CallContext> call_stack_;
    std::stack<Value> accumulator_;

    const FunctionSymbol* getFunctionByIdentifier( const std::string_view identifier ) const noexcept;
    const FunctionSymbol* getFunctionBySignature( const FunctionDefNode* node ) const noexcept;
    const FunctionSymbol* getFunctionBySignature( const BuiltinFunction& func ) const noexcept;
    const FunctionSymbol* getFunctionBySignature( const std::string_view identifier, const Type& return_type,
                                                  const std::vector<ParameterDecl>& parameters ) const noexcept;

   public:
    Environment( std::unique_ptr<const ProgramNode> program, std::vector<FunctionSymbol> functions,
                 std::stack<CallContext> call_stack );

    const std::vector<FunctionSymbol>& getFunctions() const noexcept;
    const std::stack<CallContext>& getCallStack() const noexcept;

    bool tryAddUserFunction( const FunctionDefNode* node );
    bool tryAddBuiltinFunction( BuiltinFunction function );

    bool tryAddVarOrConst( Variable variable );
    const Variable* getVarOrConstByNameCurScope( const std::string_view identifier ) const noexcept;
    Variable* getVarOrConstByNameCurScope( const std::string_view identifier ) noexcept;

    Value getRecentValFromAcc() noexcept;
    void putValInAcc( Value val ) noexcept;

    void addCallContext( CallContext::ContextType context_type );
    void popLastCallContext();

    bool isStateInWhileLoop() const noexcept;
    bool isStateInFunctionBody() const noexcept;
    const Type& getCurrentFunctionReturnType() const noexcept;

  } environment_;

  template <typename OperatorType>
  requires std::is_same_v<OperatorType, std::equal_to<>> || std::is_same_v<OperatorType, std::not_equal_to<>>
           || std::is_same_v<OperatorType, std::less<>> || std::is_same_v<OperatorType, std::less_equal<>>
           || std::is_same_v<OperatorType, std::greater<>> || std::is_same_v<OperatorType, std::greater_equal<>>
  bool evaluateRelational( const Value& left_operand, const Value& right_operad, OperatorType operatr ) const noexcept {
    return std::visit(
        [operatr]( const auto& l_op, const auto& r_op ) -> bool {
          if constexpr ( std::is_same_v<std::decay_t<decltype( l_op )>, std::decay_t<decltype( r_op )>> ) {
            return l_op == r_op;
          }
          return false;
        },
        left_operand.getData(), right_operad.getData() );
  }

 public:
  explicit Interpreter( std::unique_ptr<const ProgramNode> program, std::vector<FunctionSymbol> functions,
                        std::stack<CallContext> call_stack );

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