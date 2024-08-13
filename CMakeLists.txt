cmake_minimum_required(VERSION 3.22)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
# set_property(GLOBAL PROPERTY USE_FOLDERS ON)

project(lox LANGUAGES CXX)

add_executable(lox
	src/main.cpp
	src/lox/lexer.cpp
	src/lox/lox.cpp
	src/lox/token.cpp
)
