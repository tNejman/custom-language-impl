add_library(Interpreter
    CallStack.cc
    Interpreter.cc
    Variable.cc
)

target_link_libraries(Interpreter PUBLIC 
    Headers
    magic_enum::magic_enum
)