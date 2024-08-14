all: generate

generate:
	@mkdir -p ../src/lox/ast
	@g++ -Wall -std=c++20 astgen.cpp -o astgen
	@./astgen > ../src/lox/ast/ast.hpp

clean:
	rm -rf astgen
