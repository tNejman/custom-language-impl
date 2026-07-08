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
        Interpreter
    )
    target_compile_options(${target_name} PRIVATE -g)
    gtest_discover_tests(${target_name})
endfunction()

f_add_test_executable(I1testGoogleInterpreter I1testInterpreter.cc)
f_add_test_executable(I2testGoogleInterpreterFunctionDef I2testInterpreterFunctionDef.cc)
f_add_test_executable(I3testGoogleInterpreterVarDecl I3testInterpreterVarDecl.cc)
f_add_test_executable(I4testGoogleInterpreterIfStmt I4testInterpreterIfStmt.cc)
f_add_test_executable(I5testInterpreterWhileStmt I5testInterpreterWhileStmt.cc)
f_add_test_executable(I6testInterpreterAssignment I6testInterpreterAssignment.cc)
f_add_test_executable(I7testInterpreterBinaryExpr I7testInterpreterBinaryExpr.cc)
f_add_test_executable(I8testInterpreterUnaryExpr I8testInterpreterUnaryExpr.cc)
f_add_test_executable(I9testInterpreterCastExpr I9testInterpreterCastExpr.cc)
f_add_test_executable(I10testInterpreterLiteralExpr I10testInterpreterLiteralExpr.cc)
f_add_test_executable(I11testInterpreterPrimaryIdExpr I11testInterpreterPrimaryIdExpr.cc)
f_add_test_executable(I12testInterpreterArrSpecialOp I12testInterpreterArrSpecialOp.cc)