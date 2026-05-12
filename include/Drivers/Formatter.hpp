#pragma once

#include <format>
#include <magic_enum/magic_enum.hpp>

#include "Lexer/Token.hpp"
#include "Parser/ParameterDecl.hpp"
#include "Parser/Types.hpp"

template <>
struct std::formatter<TokenType> {
  constexpr auto parse( std::format_parse_context& ctx ) {
    return ctx.begin();
  }

  auto format( const TokenType type, format_context& ctx ) const {
    return format_to( ctx.out(), "{}", magic_enum::enum_name( type ) );
  }
};

template <>
struct std::formatter<Position> : std::formatter<std::string> {
  auto format( const Position position, format_context& ctx ) const {
    return std::formatter<std::string>::format( std::format( "Line: {}, Column: {}", position.line_, position.column_ ),
                                                ctx );
  }
};

template <>
struct std::formatter<Token> {
  constexpr auto parse( std::format_parse_context& ctx ) {
    return ctx.begin();
  }
  auto format( const Token& token, format_context& ctx ) const {
    auto out = std::format_to( ctx.out(), "Token repr. Type: {:<15} Position: {:<25}",
                               magic_enum::enum_name( token.type_ ), token.position_ );

    std::visit(
        [&out]<typename T>( const T& arg ) {
          if constexpr ( !std::same_as<T, std::monostate> ) {
            out = std::format_to( out, " Value: {}", arg );
          }
        },
        token.value_ );

    return std::format_to( out, "\n" );
  }
};

template <>
struct std::formatter<Type> {
  constexpr auto parse( std::format_parse_context& ctx ) {
    return ctx.begin();
  }

  auto format( const Type& type, format_context& ctx ) const {
    auto out = ctx.out();

    std::visit(
        [&out]<typename T>( const T& arg ) {
          if constexpr ( std::same_as<T, BaseType> ) {
            out = std::format_to( out, "{}", magic_enum::enum_name( arg ) );
          } else if constexpr ( std::same_as<T, ArrayType> ) {
            out = std::format_to( out, "{}[]", *( arg.element_type_ ) );
          }
        },
        type.internal_ );
    return out;
  }
};

template <>
struct std::formatter<ParameterDecl> {
  constexpr auto parse( std::format_parse_context& ctx ) {
    return ctx.begin();
  }

  auto format( const ParameterDecl& param_decl, format_context& ctx ) const {
    const std::string pass_mode_qunatifier = param_decl.pass_mode_ == PassMode::COPY ? "copy " : "";
    const std::string mutability_quantifier = param_decl.mutability_ == Mutability::MUTABLE ? "mut " : "";
    return std::format_to( ctx.out(), "{}{}{} {}", pass_mode_qunatifier, mutability_quantifier, param_decl.type_,
                           param_decl.name_ );
  }
};

template <>
struct std::formatter<Value> {
  constexpr auto parse( std::format_parse_context& ctx ) {
    return ctx.begin();
  }

  auto format( const Value& val, format_context& ctx ) const {
    return std::visit(
        [&]( const auto& v ) {
          using T = std::decay_t<decltype( v )>;

          if constexpr ( std::is_same_v<T, Value::ArrayValue> ) {
            auto out = std::format_to( ctx.out(), "[" );
            for ( size_t i = 0; i < v.size(); ++i ) {
              if ( i > 0 ) {
                out = std::format_to( out, ", " );
              }
              out = std::format_to( out, "{}", v[i] );
            }
            return std::format_to( out, "]" );
          } else if constexpr ( std::is_same_v<T, char> ) {
            return std::format_to( ctx.out(), "'{}'", v );
          } else if constexpr ( std::is_same_v<T, float> ) {
            return std::format_to( ctx.out(), "{:#}", v );
          } else {
            return std::format_to( ctx.out(), "{}", v );
          }
        },
        val.getData() );
  }
};