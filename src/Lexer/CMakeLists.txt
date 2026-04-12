add_library(Lexer
    Lexer.cc
)

target_include_directories(Lexer PUBLIC
    ${CMAKE_SOURCE_DIR}/include/Lexer
    ${CMAKE_SOURCE_DIR}/include
)