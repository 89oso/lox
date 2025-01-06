#include "script/parser.hpp"
#include "script/ast_json_dumper.hpp"
#include "script/interpreter.hpp"
#include "script/resolver.hpp"

#include <fstream>
#include <iostream>

namespace {
    Interpreter interpreter;

    void process_buffer(const std::string& buffer) {
        Parser parser(buffer);

        std::vector<Node::ptr>& statements = parser.parse();

        if (parser.error())
            return;

        for (auto& stmt : statements) {
            AstJsonDumper json_dumper;
            json_dumper.dump(stmt.get());
        }

        Resolver resolver(&interpreter);
        resolver.resolve_statements(statements);

        for (auto& stmt : statements) {
            interpreter.interpret(stmt.get());
        }
    }

    void process_from_file(const char* path) {
        std::ifstream file(path, std::ios::ate | std::ios::binary);

        if (!file.is_open()) {
            std::cerr << "Failed to open script: " << path << "\n";
            return;
        }

        usize size = file.tellg();
        file.seekg(0, std::ios::beg);

        std::string buffer;
        if (size > 0) {
            buffer.resize(size);
            file.read(const_cast<char*>(buffer.data()), size);
        }

        file.close();

        std::cout << "Processing script: " << path << "\n";
        process_buffer(buffer);
    }
} // namespace

int main(int argc, char** argv) {
    if (argc > 2) {
        std::cerr << "./lox [script]\n";
        return 1;
    }

    if (argc == 1) {
        std::string line;
        while (std::getline(std::cin, line)) {
            process_buffer(line);
        }
    } else {
        process_from_file(argv[1]);
    }
}
