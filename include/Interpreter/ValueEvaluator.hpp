#pragma once

#include <algorithm>
#include <cmath>
#include <functional>
#include <type_traits>
#include <utility>
#include <variant>

#include "Exceptions/InterpreterExceptions/_InterpreterExceptionInclude.hpp"
#include "Lexer/Limits.hpp"
#include "Parser/Node.h"
#include "Parser/Types.hpp"
#include "Parser/Value.hpp"

enum class ArrayBinaryOperator { CONCAT, DIFF };

class ValueEvaluator {
 private:
  template <typename... AllowedTypes>
  static void assertAllowedTypes( const Value& val ) {
    std::visit(
        []( const auto& val_unpacked ) {
          using T = std::decay_t<decltype( val_unpacked )>;
          if constexpr ( !( std::is_same_v<T, AllowedTypes> || ... ) ) {
            throw NotAllowedTypeException( Position{ 1, 1 }, "TypeError: left operand" );
          }
        },
        val.getData() );
  }
  template <typename... AllowedTypes>
  static void assertAllowedTypes( const Value& lhs, const Value& rhs ) {
    assertAllowedTypes<AllowedTypes...>( lhs );
    assertAllowedTypes<AllowedTypes...>( rhs );
  }

  static Value::ArrayValue buildConcatenatedVector( const Value::ArrayValue& first_vec,
                                                    const Value::ArrayValue& second_vec ) {
    Value::ArrayValue concatenated_arr;
    concatenated_arr.reserve( first_vec.size() + second_vec.size() );
    for ( const auto& val_inside : first_vec ) {
      concatenated_arr.push_back( val_inside.copy() );
    }
    for ( const auto& val_inside : second_vec ) {
      concatenated_arr.push_back( val_inside.copy() );
    }
    return concatenated_arr;
  }

  static Value::ArrayValue buildVectorDiff( const Value::ArrayValue& original, const Value::ArrayValue& subtracted ) {
    std::vector<std::pair<Value, int>> freq;  // count to-be-removed elems
    for ( const auto& val : subtracted ) {
      auto it = std::find_if( freq.begin(), freq.end(), [&val]( const std::pair<Value, int>& elem_ref_count_pair ) {
        return elem_ref_count_pair.first == val;
      } );
      if ( it == freq.end() ) {
        freq.emplace_back( val.copy(), 1 );
      } else {
        it->second++;
      }
    }

    Value::ArrayValue result;
    result.reserve( original.size() );
    for ( const auto& val : original ) {
      auto it = std::find_if( freq.begin(), freq.end(), [&val]( const std::pair<Value, int>& elem_ref_count_pair ) {
        return elem_ref_count_pair.first == val;
      } );
      if ( it == freq.end() ) {
        result.push_back( val.copy() );
      } else if ( it->second > 0 ) {
        result.push_back( val.copy() );
        it->second--;
      }
    }
    return result;
  }

  static Value castScalar( const Value& val, BaseType target ) {
    if ( target == BaseType::VOID ) {
      throw VoidValueException( Position{ 1, 1 }, "Cannot cast to 'void'" );
    }
    return std::visit( Overloaded{ [&]( int v ) -> Value {
                                    switch ( target ) {
                                      case BaseType::INT: return v;
                                      case BaseType::FLOAT: return static_cast<float>( v );
                                      case BaseType::CHAR: return static_cast<char>( v );
                                      case BaseType::BOOL: return v != 0;
                                      case BaseType::VOID: std::unreachable();
                                    }
                                  },
                                   [&]( float v ) -> Value {
                                     switch ( target ) {
                                       case BaseType::INT: return static_cast<int>( v );
                                       case BaseType::FLOAT: return v;
                                       case BaseType::CHAR:
                                         throw InvalidCastException( Position{ 1, 1 }, "cannot cast float to char" );
                                       case BaseType::BOOL: return v != 0.0f;
                                       case BaseType::VOID: std::unreachable();
                                     }
                                   },
                                   [&]( char v ) -> Value {
                                     switch ( target ) {
                                       case BaseType::INT: return static_cast<int>( v );
                                       case BaseType::FLOAT:
                                         throw InvalidCastException( Position{ 1, 1 }, "cannot cast char to float" );
                                       case BaseType::CHAR: return v;
                                       case BaseType::BOOL:
                                         throw InvalidCastException( Position{ 1, 1 }, "cannot cast char to bool" );
                                       case BaseType::VOID: std::unreachable();
                                     }
                                   },
                                   [&]( bool v ) -> Value {
                                     switch ( target ) {
                                       case BaseType::INT: return v ? 1 : 0;
                                       case BaseType::FLOAT: return v ? 1.0f : 0.0f;
                                       case BaseType::CHAR:
                                         throw InvalidCastException( Position{ 1, 1 }, "cannot cast bool to char" );
                                       case BaseType::BOOL: return v;
                                       case BaseType::VOID: std::unreachable();
                                     }
                                   },
                                   []( const Value::ArrayValue& ) -> Value {
                                     throw InvalidCastException( Position{ 1, 1 },
                                                                 "Cannot cast ArrayValue, is not a scalar" );
                                   } },
                       val.getData() );
  }

 public:
  static Value evaluateLogicalOr( const Value& lhs, const Value& rhs ) {
    assertAllowedTypes<bool>( lhs, rhs );
    return Value{ std::get<bool>( lhs.getData() ) || std::get<bool>( lhs.getData() ) };
  }

  static Value evaluateLogicalAnd( const Value& lhs, const Value& rhs ) {
    assertAllowedTypes<bool>( lhs, rhs );
    return Value{ std::get<bool>( lhs.getData() ) && std::get<bool>( lhs.getData() ) };
  }

  template <typename OperatorType>
  requires std::is_same_v<OperatorType, std::less<>> || std::is_same_v<OperatorType, std::less_equal<>>
           || std::is_same_v<OperatorType, std::greater<>> || std::is_same_v<OperatorType, std::greater_equal<>>
  static Value evaluateRelational( const Value& lhs, const Value& rhs, OperatorType op ) {
    return std::visit( Overloaded{ [op]( const int l_op, const int r_op ) -> bool { return op( l_op, r_op ); },
                                   [op]( const float l_op, const float r_op ) -> bool { return op( l_op, r_op ); },
                                   [op]( const char l_op, const char r_op ) -> bool { return op( l_op, r_op ); },
                                   []( const auto&, const auto& ) -> bool {
                                     throw NotAllowedTypeException( Position{ 1, 1 },
                                                                    "can only compare int,float,char" );
                                   } },
                       lhs.getData(), rhs.getData() );
  }

  struct FmodModulus {
    constexpr auto operator()( int x, int y ) const {
      return x % y;
    }
    constexpr auto operator()( float x, float y ) const {
      float res = std::fmod( x, y );
      float relative_epsilon = std::max( std::abs( x ), std::abs( y ) ) * 1e-6f;
      if ( std::abs( res ) < relative_epsilon ) {
        return 0.0f;
      }
      if ( std::abs( std::abs( res ) - std::abs( y ) ) < relative_epsilon ) {
        return 0.0f;
      }

      return res;
    }
  };

  enum class NumericOp { ADD, SUB, MUL, DIV, MOD };

  static Value evaluateNumeric( const Value& lhs, const Value& rhs, NumericOp op ) {
    auto clamp_to_int = []( long long val ) -> int {
      return static_cast<int>( std::clamp<long long>( val, tkom_limits::MIN_INT, tkom_limits::MAX_INT ) );
    };
    auto clamp_to_float = []( float val ) -> float {
      return std::clamp<float>( val, tkom_limits::MIN_FLOAT, tkom_limits::MAX_FLOAT );
    };
    return std::visit(
        Overloaded{ [&]( const int l, const int r ) -> Value {
                     switch ( op ) {
                       case NumericOp::ADD:
                         return clamp_to_int( static_cast<long long>( l ) + static_cast<long long>( r ) );
                       case NumericOp::SUB:
                         return clamp_to_int( static_cast<long long>( l ) - static_cast<long long>( r ) );
                       case NumericOp::MUL:
                         return clamp_to_int( static_cast<long long>( l ) * static_cast<long long>( r ) );
                       case NumericOp::DIV: throw InvalidOperationException( Position{ 1, 1 }, "cannot divide ints" );
                       case NumericOp::MOD:
                         if ( r == 0 ) {
                           throw InvalidOperationException( Position{ 1, 1 }, "cannot mod by 0" );
                         } else {
                           return clamp_to_int( static_cast<long long>( l ) % static_cast<long long>( r ) );
                         }
                       default: std::unreachable();
                     }
                   },
                    [&]( const float l, const float r ) -> Value {
                      switch ( op ) {
                        case NumericOp::ADD: return clamp_to_float( l + r );
                        case NumericOp::SUB: return clamp_to_float( l - r );
                        case NumericOp::MUL: return clamp_to_float( l * r );
                        case NumericOp::DIV:
                          if ( r == 0.f ) {
                            throw InvalidOperationException( Position{ 1, 1 }, "cannot divide by 0" );
                          } else {
                            return clamp_to_float( l / r );
                          }
                        case NumericOp::MOD:
                          if ( r == 0.f ) {
                            throw InvalidOperationException( Position{ 1, 1 }, "cannot mod by 0" );
                          } else {
                            return clamp_to_float( FmodModulus{}( l, r ) );
                          }
                        default: std::unreachable();
                      }
                    },
                    []( const auto&, const auto& ) -> Value {
                      throw InvalidOperationException(
                          Position{ 1, 1 }, "numeric operations are only legal on 'int-int' and 'float-float' pairs" );
                    } },
        lhs.getData(), rhs.getData() );
  }

  static Value evaluateArrOp( const Value& lhs, const Value& rhs, ArrayBinaryOperator op ) {
    assertAllowedTypes<Value::ArrayValue>( lhs, rhs );
    const Value::ArrayValue& lhs_arr = std::get<Value::ArrayValue>( lhs.getData() );
    const Value::ArrayValue& rhs_arr = std::get<Value::ArrayValue>( lhs.getData() );
    switch ( op ) {
      case ArrayBinaryOperator::CONCAT: {
        return Value{ buildConcatenatedVector( lhs_arr, rhs_arr ) };
      }
      case ArrayBinaryOperator::DIFF: {
        return Value{ buildVectorDiff( lhs_arr, rhs_arr ) };
      }
    }
  }

  static Value evaluateArrAccess( const Value& lhs, const Value& rhs ) {
    assertAllowedTypes<Value::ArrayValue>( lhs );
    assertAllowedTypes<int>( rhs );
    const Value::ArrayValue& array = std::get<Value::ArrayValue>( lhs.getData() );
    const int idx = std::get<int>( rhs.getData() );
    if ( idx < 0 ) {
      throw IndexOutOfBoundsException( Position{ 1, 1 }, "Array index must be positive" );
    }
    if ( array.size() >= static_cast<size_t>( idx ) ) {
      throw IndexOutOfBoundsException( Position{ 1, 1 }, "Array index exceeding array size" );
    }
    return array[static_cast<size_t>( idx )].copy();
  }

  static Value evaluateUnaryOp( const Value& operand, UnaryOperator op ) {
    auto clamp_to_int = []( long long val ) -> int {
      return static_cast<int>( std::clamp<long long>( val, tkom_limits::MIN_INT, tkom_limits::MAX_INT ) );
    };
    auto clamp_to_float = []( float val ) -> float {
      return std::clamp<float>( val, tkom_limits::MIN_FLOAT, tkom_limits::MAX_FLOAT );
    };
    switch ( op ) {
      case UnaryOperator::LEN: {
        assertAllowedTypes<Value::ArrayValue>( operand );
        const auto& arr = std::get<Value::ArrayValue>( operand.getData() );
        return Value{ static_cast<int>( arr.size() ) };
      }
      case UnaryOperator::REV: {
        assertAllowedTypes<Value::ArrayValue>( operand );
        const auto& arr = std::get<Value::ArrayValue>( operand.getData() );
        return [&] {
          Value::ArrayValue new_arr;
          new_arr.reserve( arr.size() );
          for ( const auto& val : arr ) {
            new_arr.push_back( val.copy() );
          }
          std::reverse( new_arr.begin(), new_arr.end() );
          return Value{ std::move( new_arr ) };
        }();
      }
      case UnaryOperator::NEG: {
        assertAllowedTypes<int, float>( operand );
        return std::visit(
            Overloaded{ [&]( const int val ) -> Value { return clamp_to_int( -static_cast<long long>( val ) ); },
                        [&]( const float val ) -> Value { return clamp_to_float( -val ); },
                        []( auto&& _ ) -> Value { std::unreachable(); } },
            operand.getData() );
      }
      case UnaryOperator::NOT: {
        assertAllowedTypes<bool>( operand );
        return Value{ !std::get<bool>( operand.getData() ) };
      }
    }
  };

  static Value evaluateCastOp( const Value& operand, const Type& type_cast_to ) {
    bool target_is_array = std::holds_alternative<ArrayType>( type_cast_to.internal_ );
    const auto& op_data = operand.getData();
    bool operand_is_array = std::holds_alternative<Value::ArrayValue>( op_data );

    if ( !operand_is_array && !target_is_array ) {
      return castScalar( operand, std::get<BaseType>( type_cast_to.internal_ ) );
    }
    if ( !operand_is_array || !target_is_array ) {
      throw InvalidCastException( Position{ 1, 1 }, "order incompatiblity scalar<->arr when casting" );
    }
    const auto& arr = std::get<Value::ArrayValue>( op_data );
    const auto& target_arr_type = std::get<ArrayType>( type_cast_to.internal_ );

    Value::ArrayValue new_arr;
    new_arr.reserve( arr.size() );

    for ( const auto& val : arr ) {
      new_arr.push_back( evaluateCastOp( val, *target_arr_type.element_type_ ) );
    }
    return Value( std::move( new_arr ) );
  }
};