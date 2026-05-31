#pragma once

#include <functional>
#include <vector>

#include "Interpreter/Variable.h"
#include "Parser/Node.h"

class CallContext {
 public:
  enum class ContextType { TOP_LEVEL, IF_BLOCK, FUNCTION_CALL, WHILE_BLOCK };

 private:
  ContextType type_;
  std::vector<Variable> variables_;
  const FunctionDefNode* function_sig_;

  struct TraceWrapper {};

 public:
  CallContext( ContextType context_type ) noexcept;
  CallContext( const FunctionDefNode* func_sig ) noexcept;
  ContextType getType() const noexcept;
  void addVariable( Variable variable ) noexcept;
  const std::vector<Variable>& getVariables() const noexcept;
  std::vector<Variable>& getVariables() noexcept;
  const FunctionDefNode* getFunctionSig() const noexcept;
};

class CallStack {
 private:
  std::vector<CallContext> call_stack_;

 public:
  CallContext pop();
  void push( CallContext );
  CallContext& top() noexcept;
  const CallContext& top() const noexcept;
  std::optional<std::reference_wrapper<CallContext>> nth( size_t idx ) noexcept;
  const std::vector<CallContext>& view() const noexcept;
  bool empty() const noexcept;
  size_t size() const noexcept;
};