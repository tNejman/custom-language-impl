#pragma once

#include <format>
#include <magic_enum/magic_enum.hpp>

#include "Lexer/Token.hpp"

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