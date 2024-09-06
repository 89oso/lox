all: generate

generate:
	@mkdir -p ../src/ast
	@g++ -Wall -std=c++20 astgen.cpp -o astgen
	@./astgen

clean:
	rm -rf astgen
