add_library(Lexer STATIC
    Lexer.cc
)

target_link_libraries(Lexer PUBLIC 
    Headers
    magic_enum::magic_enum
)