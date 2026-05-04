add_library(Parser
    Parser.cc
)

target_link_libraries(Lexer PUBLIC 
    Headers
)