include(GoogleTest)

set(CMAKE_THREAD_PREFER_PTHREAD TRUE)
set(THREADS_PREFER_PTHREAD_FLAG TRUE)

add_executable(testGoogleLexer testLexer.cc ${PROJECT_SOURCES})
add_executable(testGoogleKeywordsAndIdentifiers testLexerKeywordsAndIdentifiers.cc ${PROJECT_SOURCES})
add_executable(testGoogleLexerLiteral testLexerLiterals.cc ${PROJECT_SOURCES})
add_executable(testGoogleLexerPositions testLexerPositions.cc ${PROJECT_SOURCES})

set(MODULES
    testGoogleLexer
    testGoogleKeywordsAndIdentifiers
    testGoogleLexerLiteral
    testGoogleLexerPositions
)

foreach(module ${MODULES})
    target_link_libraries(${module} PUBLIC
        GTest::gtest_main
        Threads::Threads
        Lexer
    )

    target_include_directories(${module} PUBLIC
        ${gtest_SOURCE_DIR}/include
        ${CMAKE_SOURCE_DIR}/include/Lexer
        ${CMAKE_SOURCE_DIR}/include/Exceptions/LexerExceptions
        ${CMAKE_SOURCE_DIR}/include
        ${CMAKE_SOURCE_DIR}/test
    )
    
    gtest_discover_tests(${module})
endforeach()
