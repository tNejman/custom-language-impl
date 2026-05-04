function(f_add_executable target_name file_name)
    add_executable(${target_name} ${file_name})
    apply_compiler_flags(${target_name})  
    target_link_libraries(${target_name} PUBLIC
        ${MODULES}
        Headers
        magic_enum::magic_enum
        argparse::argparse
    )
endfunction()

f_add_executable(LexerApp LexerApp.cc)
f_add_executable(Main Main.cc)