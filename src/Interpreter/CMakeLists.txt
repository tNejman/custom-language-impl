add_library(Interpreter STATIC
    CallStack.cc
    Interpreter.cc
    RuntimeValue.cc
    Variable.cc
)

target_link_libraries(Interpreter PUBLIC 
    Headers
    magic_enum::magic_enum
)