#include "script/parser.hpp"
#include "script/ast_json_dumper.hpp"
#include "script/interpreter.hpp"

#include <fstream>
#include <iostream>

namespace {
    Interpreter interpreter;

    void process_buffer(const std::string& buffer) {
        Parser parser(buffer);

        Node::ptr root = parser.parse();

        if (parser.error())
            return;

        AstJsonDumper json_dumper;
        std::cout << json_dumper.dump(root.get());

        interpreter.interpret(root.get());
    }

    void process_from_file(const char* path) {
        std::ifstream file(path, std::ios::ate | std::ios::binary);

        if (!file.is_open()) {
            std::cerr << "Failed to open script: " << path << "\n";
            return;
        }

        size_t size = file.tellg();
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
