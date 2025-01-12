cmake_minimum_required(VERSION 3.23)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include(FetchContent)

FetchContent_Declare(
    fmt
    GIT_REPOSITORY https://github.com/fmtlib/fmt
    GIT_TAG        e69e5f977d458f2650bb346dadf2ad30c5320281) # 10.2.1

FetchContent_MakeAvailable(fmt)

project(jlox LANGUAGES CXX)

add_executable(jlox
    src/main.cpp
    src/common/memory/arena.cpp
    src/common/utils.cpp
    src/script/ast.cpp
    src/script/ast_dumper.cpp
    src/script/callable.cpp
    src/script/environment.cpp
    src/script/function.cpp
    src/script/interpreter.cpp
    src/script/lexer.cpp
    src/script/object.cpp
    src/script/parser.cpp
    src/script/resolver.cpp
    src/script/token.cpp
)

target_include_directories(jlox PRIVATE include/)
target_link_libraries(jlox fmt::fmt)
