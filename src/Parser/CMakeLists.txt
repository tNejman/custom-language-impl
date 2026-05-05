add_library(Parser
    Parser.cc
    Variable.cc
)

target_link_libraries(Parser PUBLIC 
    Headers
)