cmake_minimum_required(VERSION 3.25)
project(TKOM_project LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON) 
set(CMAKE_EXPORT_COMPILE_COMMANDS ON) 
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin) 

find_package(Threads REQUIRED)

set(PROJ_HEADER_FILTER "^${CMAKE_SOURCE_DIR}/(include|src)/")
find_program(CLANG_TIDY_EXE NAMES clang-tidy-19 clang-tidy-18 clang-tidy)
if(NOT CLANG_TIDY_EXE)
    message(WARNING "clang-tidy not found. Linting disabled.")
endif()

if(CMAKE_CXX_COMPILER_ID MATCHES "Clang|GNU")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -w -D_CRT_SECURE_NO_WARNINGS")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -w -D_CRT_SECURE_NO_WARNINGS")
endif()

include(FetchContent)

FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG v1.17.0
    SYSTEM
)
FetchContent_MakeAvailable(googletest)

FetchContent_Declare(
    magic_enum
    GIT_REPOSITORY https://github.com/Neargye/magic_enum.git
    GIT_TAG v0.9.7
    SYSTEM
)
FetchContent_MakeAvailable(magic_enum)

FetchContent_Declare(
    argparse
    GIT_REPOSITORY https://github.com/p-ranav/argparse.git
    GIT_TAG v3.2
    SYSTEM
)
FetchContent_MakeAvailable(argparse)

set(MODULES
    Lexer
    Parser
    Drivers
)

add_library(Headers INTERFACE)
target_include_directories(Headers INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/include)

function(apply_compiler_flags target)
    if(TARGET ${target})
        target_compile_options(${target} PRIVATE
            -g -O0
            -Wall -Wextra -Wpedantic -Werror 
            -Wcast-align=strict -Wcast-qual 
            -Wconversion -Wsign-conversion -Wshadow 
            -Wdouble-promotion -Wnull-dereference 
            -Wformat=2 -Wduplicated-cond -Wduplicated-branches 
            -Wlogical-op -Wuseless-cast -Wzero-as-null-pointer-constant 
            -Wvla -Wshift-overflow=2 -Wstringop-overflow=4 
        )
    else()
        message(WARNING "Target '${target}' not found - skipping compiler flags")
    endif()
endfunction()

foreach(module ${MODULES})
    add_subdirectory(src/${module})
    apply_compiler_flags(${module})
endforeach()

add_subdirectory(app)

enable_testing()
add_subdirectory(test)