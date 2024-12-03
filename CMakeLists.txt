cmake_minimum_required(VERSION 3.22)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project(lox LANGUAGES CXX)

add_executable(lox
    src/main.cpp
    src/common/memory/arena.cpp
    src/common/utils.cpp
    src/script/lexer.cpp
    src/script/parser.cpp
    src/script/token.cpp
)

target_include_directories(lox PRIVATE include/)
