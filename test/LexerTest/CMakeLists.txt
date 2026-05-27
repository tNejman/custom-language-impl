include(GoogleTest)

set(CMAKE_THREAD_PREFER_PTHREAD TRUE)
set(THREADS_PREFER_PTHREAD_FLAG TRUE)

function(f_add_test_executable target_name file_name)
    add_executable(${target_name} ${file_name} ${PROJECT_SOURCES})
    target_link_libraries(${target_name} PUBLIC
        GTest::gtest_main
        Threads::Threads
        Lexer
    )
    gtest_discover_tests(${target_name})
endfunction()

f_add_test_executable(testGoogleLexer testLexer.cc)
f_add_test_executable(testGoogleKeywordsAndIdentifiers testLexerKeywordsAndIdentifiers.cc)
f_add_test_executable(testGoogleLexerLiteral testLexerLiterals.cc)
f_add_test_executable(testGoogleLexerPositions testLexerPositions.cc)