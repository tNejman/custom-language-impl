add_executable(LexerApp LexerApp.cc)
apply_compiler_flags(LexerApp)

add_executable(Main Main.cc)
apply_compiler_flags(Main)

set(EXECUTABLE_TARGETS LexerApp Main)

foreach(exec ${EXECUTABLE_TARGETS})
    target_include_directories(${exec} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)    
    target_link_libraries(${exec} PRIVATE
    ${MODULES}
    magic_enum::magic_enum
    argparse::argparse
)
endforeach()