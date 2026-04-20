cmake_minimum_required(VERSION 3.25)
project(TKOM_project LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON) 
set(CMAKE_EXPORT_COMPILE_COMMANDS ON) 
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin) 

# find_package(TBB REQUIRED)
find_package(Threads REQUIRED)

set(PROJ_HEADER_FILTER "^${CMAKE_SOURCE_DIR}/(include|src)/")
find_program(CLANG_TIDY_EXE NAMES clang-tidy-19 clang-tidy-18 clang-tidy)
if(NOT CLANG_TIDY_EXE)
    message(WARNING "clang-tidy not found. Linting disabled.")
endif()

# set(BACKUP_C_FLAGS "${CMAKE_C_FLAGS}")
# set(BACKUP_CXX_FLAGS "${CMAKE_CXX_FLAGS}")

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

# set(EXTERNAL_TARGETS gtest gtest_main gmock gmock_main)

# foreach(t ${EXTERNAL_TARGETS})
#     if(TARGET ${t})
#         set_target_properties(${t} PROPERTIES CXX_CLANG_TIDY "")
#         get_target_property(inc_dirs ${t} INTERFACE_INCLUDE_DIRECTORIES)
        
#         if(inc_dirs)
#             set_target_properties(${t} PROPERTIES
#                 INTERFACE_SYSTEM_INCLUDE_DIRECTORIES "${inc_dirs}")
#         endif()
#     endif()
# endforeach()

# set(CMAKE_C_FLAGS "${BACKUP_C_FLAGS}")
# set(CMAKE_CXX_FLAGS "${BACKUP_CXX_FLAGS}")

set(MODULES
    Lexer
)

add_library(Headers INTERFACE)
target_include_directories(Headers INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/include)

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

enable_testing()
add_subdirectory(test)