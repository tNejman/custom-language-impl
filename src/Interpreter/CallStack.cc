#include "Interpreter/CallStack.h"

#include <optional>

/* CallContext


*/

CallContext::CallContext( ContextType context_type ) noexcept
    : type_( context_type ), variables_(), function_sig_( std::nullopt ) {
  assert( context_type != ContextType::FUNCTION_CALL && "function call context must be initialized with function ptr" );
}
CallContext::CallContext( const FunctionDefNode& func_sig ) noexcept
    : type_( ContextType::FUNCTION_CALL ), variables_(), function_sig_( func_sig ) {
}
CallContext::ContextType CallContext::getType() const noexcept {
  return type_;
}
void CallContext::addVariable( Variable variable ) noexcept {
  variables_.push_back( std::move( variable ) );
}
const std::vector<Variable>& CallContext::getVariables() const noexcept {
  return variables_;
}
std::vector<Variable>& CallContext::getVariables() noexcept {
  return variables_;
}
std::optional<std::reference_wrapper<const FunctionDefNode>> CallContext::getFunctionSig() const noexcept {
  return function_sig_;
}

/* CallStack



*/

CallContext CallStack::pop() {
  CallContext call_context = std::move( call_stack_.back() );
  call_stack_.pop_back();
  return call_context;
}
void CallStack::push( CallContext call_context ) {
  call_stack_.push_back( std::move( call_context ) );
}
CallContext& CallStack::top() noexcept {
  return call_stack_.back();
}
const CallContext& CallStack::top() const noexcept {
  return call_stack_.back();
}
std::optional<std::reference_wrapper<CallContext>> CallStack::nth( size_t idx ) noexcept {
  if ( call_stack_.size() >= idx ) {
    return std::nullopt;
  }
  return call_stack_[idx];
}
std::optional<std::reference_wrapper<const CallContext>> CallStack::nth( size_t idx ) const noexcept {
  if ( call_stack_.size() >= idx ) {
    return std::nullopt;
  }
  return call_stack_[idx];
}

const std::vector<CallContext>& CallStack::view() const noexcept {
  return call_stack_;
}
bool CallStack::empty() const noexcept {
  return call_stack_.empty();
}
size_t CallStack::size() const noexcept {
  return call_stack_.size();
}