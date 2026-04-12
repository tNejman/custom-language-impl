cmake_minimum_required(VERSION 3.16)
project(TKOM_project LANGUAGES CXX)
set(APP_NAME TKOM_project)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON) 
set(CMAKE_EXPORT_COMPILE_COMMANDS ON) 
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin) 

find_package(TBB REQUIRED)
find_package(Threads REQUIRED)

include(FetchContent)

FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG v1.17.0
)
FetchContent_MakeAvailable(googletest)

set(MODULES
    Lexer
)

function(apply_compiler_flags target)
    if(TARGET ${target})
        if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU") 
            target_compile_options(${target} PRIVATE
                -g -O0
                -Wall -Wextra -Wpedantic -Werror 
                -Wcast-align=strict -Wcast-qual 
                -Wconversion -Wsign-conversion -Wshadow 
                -Wdouble-promotion -Wnull-dereference 
                -Wformat=2 -Wduplicated-cond -Wduplicated-branches 
                -Wlogical-op -Wuseless-cast -Wzero-as-null-pointer-constant 
                -Wvla -Wshift-overflow=2 -Wstringop-overflow=4 
                -Wnarrowing -Wuninitialized 
            )
        elseif(CMAKE_CXX_COMPILER_ID STREQUAL "Clang") 
            target_compile_options(${target} PRIVATE
                -g -O0
                -Wall -Wextra -Wpedantic -Werror 
                -Wcast-align -Wcast-qual 
                -Wconversion -Wsign-conversion -Wshadow 
                -Wdouble-promotion -Wnull-dereference 
                -Wformat -Wformat-pedantic 
                -Wzero-as-null-pointer-constant 
                -Wvla -Wshift-overflow -Wnarrowing -Wuninitialized 
            )
        endif()
    else()
        message(WARNING "Target '${target}' not found - skipping compiler flags")
    endif()
endfunction()

foreach(module ${MODULES})
    add_subdirectory(src/${module})
    apply_compiler_flags(${module})
endforeach()

add_subdirectory(app)
apply_compiler_flags(${APP_NAME})

enable_testing()
add_subdirectory(test)