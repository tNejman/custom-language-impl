include(GoogleTest)

set(CMAKE_THREAD_PREFER_PTHREAD TRUE)
set(THREADS_PREFER_PTHREAD_FLAG TRUE)

function(f_add_test_executable target_name file_name)
    add_executable(${target_name} ${file_name} ${PROJECT_SOURCES})
    target_link_libraries(${target_name} PUBLIC
        GTest::gtest_main
        Threads::Threads
        magic_enum::magic_enum
        Lexer
        Parser
    )
    target_compile_options(${target_name} PRIVATE -g)
    gtest_discover_tests(${target_name})
endfunction()

f_add_test_executable(1testGoogleParser 1testParser.cc)
f_add_test_executable(2testGoogleParserFunctions 2testParserFunctions.cc)
f_add_test_executable(3testGoogleParserVariableDeclarations 3testParserVariableDeclarations.cc)
f_add_test_executable(4testGoogleParserIfStatement 4testParserIfStatement.cc)
f_add_test_executable(5testGoogleParserWhileAndLoopControl 5testParserWhileAndLoopControl.cc)
f_add_test_executable(6testGoogleParserOperators 6testParserOperators.cc)
f_add_test_executable(7testGoogleParserLiterals 7testParserLiterals.cc)