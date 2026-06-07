#pragma once

#include <gtest/gtest.h>

#include <print>

// #include "MockParser.hpp"
#include "Interpreter/CallStack.h"
#include "Interpreter/Interpreter.h"
#include "Parser/Node.h"
class InterpreterTest : public ::testing::Test {
 protected:
  void SetUp() override {
  }

  void TearDown() override {
  }
};

class InterpreterTestFriend {
 public:
  static auto& env( Interpreter& i ) {
    return i.environment_;
  };
  static auto& acc( Interpreter& i ) {
    return i.accumulator_;
  }
  static auto& funcs( Interpreter& i ) {
    return i.environment_.functions_;
  }
  static auto& callStack( Interpreter& i ) {
    return i.environment_.call_stack_;
  }
  static auto& loopC( Interpreter& i ) {
    return i.environment_.loop_control_type_;
  }
  static auto& varsTop( Interpreter& i ) {
    return i.environment_.call_stack_.top().getVariables();
  }
  static auto& varsGlob( Interpreter& i ) {
    return i.environment_.call_stack_.nth( 0u ).value().get().getVariables();
  }
  static void addMockCallContext( Interpreter& i, CallContext::ContextType context_type,
                                  size_t expect_variable_count ) {
    auto& environment = env( i );
    environment.call_stack_.push( CallContext{ context_type, expect_variable_count } );
  }
  static void addMockCallArg( Interpreter& i, Value v ) {
    acc( i ).push( RuntimeValue{ std::move( v ) } );
  }
};

using ITF = InterpreterTestFriend;

template <typename... IgnoredNodes>
bool isIgnoredType( const INode& node ) {
  return ( ( dynamic_cast<const IgnoredNodes*>( &node ) != nullptr ) || ... );
}

void assertCallStackSize( Interpreter& i, size_t expected_size ) {
  const auto& call_stack = ITF::callStack( i );
  ASSERT_EQ( expected_size, call_stack.size() );
}

void assertTopCallContextType( Interpreter& i, CallContext::ContextType type ) {
  const auto& call_stack = ITF::callStack( i );
  ASSERT_EQ( type, call_stack.top().getType() );
}

void assertAccSize( Interpreter& i, size_t expected_size ) {
  const auto& acc = ITF::acc( i );
  ASSERT_EQ( expected_size, acc.size() );
}

void assertAccTopVal( Interpreter& i, bool is_void, const auto& val ) {
  const auto& acc = ITF::acc( i );
  ASSERT_EQ( is_void, acc.top().isVoid() );
  // std::print( "\n\n\n{}, {}\n\n\n", acc.top().copyValue(), val );
  if ( !is_void ) {
    ASSERT_EQ( acc.top().copyValue(), val );
  }
}

void assertTopCallContextVarCount( Interpreter& i, size_t expected_count ) {
  const auto& top_call_context = ITF::callStack( i ).top();
  ASSERT_EQ( expected_count, top_call_context.getVariables().size() );
}

#define MAKE_LITERAL( type, val ) std::make_unique<LiteralExprNode>( Position{ 1, 1 }, type, val )

#define MAKE_ARR_LITERAL( ... ) std::make_unique<ArrayLiteralNode>( Position{ 1, 1 }, makeExpressions( __VA_ARGS__ ) )

#define MAKE_ID( id ) std::make_unique<PrimaryIdentifierNode>( Position{ 1, 1 }, id )

#define MAKE_DECL_VAR( id, type, val ) \
  std::make_unique<VarOrConstDeclNode>( Position{ 1, 1 }, id, Mutability::MUTABLE, type, val )

#define MAKE_DECL_CONST( id, type, val ) \
  std::make_unique<VarOrConstDeclNode>( Position{ 1, 1 }, id, Mutability::IMMUTABLE, type, val )

#define MAKE_VAR_INT( id, val ) MAKE_DECL_VAR( id, BaseType::INT, MAKE_LITERAL( BaseType::INT, val ) )

#define MAKE_CONST_INT( id, val ) MAKE_DECL_CONST( id, BaseType::INT, MAKE_LITERAL( BaseType::INT, val ) )

#define MAKE_ASSIGNMENT_EXPR( lhs, rhs ) \
  std::make_unique<AssignmentExprNode>( Position{ 1, 1 }, lhs, rhs, AssignmentType::ASSIGN )

#define MAKE_ASSIGNMENT_EXPR_WITH_OP( lhs, rhs, op ) \
  std::make_unique<AssignmentExprNode>( Position{ 1, 1 }, lhs, rhs, op )

#define MAKE_BINARY_EXPR( lhs, rhs, op ) std::make_unique<BinaryExprNode>( Position{ 1, 1 }, lhs, rhs, op )

#define MAKE_UNARY_EXPR( operand, operator) std::make_unique<UnaryExprNode>( Position{ 1, 1 }, operand, operator)

#define MAKE_CAST_EXPR( operand, type_cast_to ) \
  std::make_unique<CastExprNode>( Position{ 1, 1 }, operand, type_cast_to )

#define MAKE_IF( cond_block_pairs, else_block ) \
  std::make_unique<IfStatementNode>( Position{ 1, 1 }, cond_block_pairs, else_block )

#define MAKE_WHILE( cond, block ) std::make_unique<WhileStatementNode>( Position{ 1, 1 }, cond, block )

#define MAKE_BREAK() std::make_unique<ControlFlowNode>( Position{ 1, 1 }, ControlFlowType::BREAK )
#define MAKE_CONTINUE() std::make_unique<ControlFlowNode>( Position{ 1, 1 }, ControlFlowType::CONTINUE )
#define MAKE_RETURN( ret_val ) std::make_unique<ReturnNode>( Position{ 1, 1 }, ret_val )

#define MAKE_ARR_OP_SPEC(arr,type, id) std::make_unique<ArrayIdentifierOpNode>(Position{1,1}, arr, type, id)

template <typename Container, typename... Args>
Container makeContainer( Args&&... args ) {
  Container c;
  c.reserve( sizeof...( args ) );
  ( c.push_back( std::move( args ) ), ... );
  return c;
}

template <typename... Args>
Block makeBlock( Args&&... args ) {
  return makeContainer<Block>( std::move( args )... );
}

template <typename... Args>
std::vector<std::unique_ptr<INode>> makeStatements( Args&&... args ) {
  return makeContainer<std::vector<std::unique_ptr<INode>>>( std::move( args )... );
}

template <typename... Args>
std::vector<std::unique_ptr<FunctionDefNode>> makeFunctions( Args&&... args ) {
  return makeContainer<std::vector<std::unique_ptr<FunctionDefNode>>>( std::move( args )... );
}

template <typename... Args>
std::vector<ParameterDecl> makeParams( Args&&... args ) {
  return makeContainer<std::vector<ParameterDecl>>( std::move( args )... );
}

template <typename... Args>
std::vector<std::unique_ptr<IExpressionNode>> makeExpressions( Args&&... args ) {
  return makeContainer<std::vector<std::unique_ptr<IExpressionNode>>>( std::move( args )... );
}

template <typename... Args>
ExprBlockPairVec makeExprBlockPairVec( Args&&... args ) {
  static_assert( sizeof...( Args ) % 2 == 0 );

  return []<size_t... Is>( auto&& t, std::index_sequence<Is...> ) {
    return makeContainer<ExprBlockPairVec>(
        std::pair{ std::move( std::get<2 * Is>( t ) ), std::move( std::get<2 * Is + 1>( t ) ) }... );
  }( std::forward_as_tuple( std::forward<Args>( args )... ), std::make_index_sequence<sizeof...( Args ) / 2>{} );
}

#define MAKE_STATEMENTS( ... )    \
  auto statements {               \
    makeStatements( __VA_ARGS__ ) \
  }

#define MAKE_FUNCTIONS( ... )    \
  auto functions {               \
    makeFunctions( __VA_ARGS__ ) \
  }

#define IT interpreter

#define MAKE_INTERPRETER \
  Interpreter IT{ std::make_unique<ProgramNode>( Position{ 1, 1 }, std::move( statements ), std::move( functions ) ) };

template <typename... SoughtTypes, typename... ExpectedCounts>
void executeAndAssertNodeVisits( Interpreter& IT, ExpectedCounts... expected_counts ) {
  static_assert( sizeof...( SoughtTypes ) == sizeof...( ExpectedCounts ) );

  std::array<int, sizeof...( ExpectedCounts )> expected = { static_cast<int>( expected_counts )... };
  std::array<int, sizeof...( SoughtTypes )> actual = { 0 };

  std::array<const char*, sizeof...( SoughtTypes )> type_names = { typeid( SoughtTypes ).name()... };

  IT.setDebugHook( [&actual]( Interpreter& interpreter, const INode& node, DebugEvent event ) {
    std::size_t index = 0;

    if ( event == DebugEvent::BEFORE_NODE_VISIT ) {  // count enters only
      static_cast<void>(
          ( ... || ( dynamic_cast<const SoughtTypes*>( &node ) ? ( ++actual[index], true ) : ( ++index, false ) ) ) );
    }
  } );

  IT.execute();

  for ( std::size_t i = 0; i < expected.size(); ++i ) {
    std::println( "expecting: {}", type_names[i] );
    ASSERT_EQ( expected[i], actual[i] );
  }
}

// #define MAKE_HOOK( ... )                                                            \
//   IT.setDebugHook( [&]( Interpreter & IT, const INode& node, DebugEvent event )  { \
//     __VA_ARGS__                                                                     \
//   })

// #define MAKE_HOOK_M( skips_enc, ... ) \
//   IT.setDebugHook(                    \
//       [skips = ( skips_enc )]( Interpreter& IT, const INode& node, DebugEvent event ) mutable { __VA_ARGS__ } )