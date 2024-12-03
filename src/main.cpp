#include "script/parser.hpp"
#include "script/ast/ast_printer.hpp"
#include "script/ast/ast_json_dumper.hpp"

#include <fstream>
#include <iostream>

static void process_buffer(const std::string& buffer) {
    Parser parser(buffer);

    Node::ptr root = parser.parse();

    if (parser.error())
        return;

    ASTPrinter p;
    p.print(root.get());

    AstJsonDumper ast_json_dumper;
    std::cout << ast_json_dumper.dump(root.get());
}

static void process_from_file(const char* path) {
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

int main(int argc, char** argv) {
    if (argc > 2) {
        std::cerr << "./lox [script]\n";
        return 1;
    }

    if (argc == 1) {
        std::string line;
        std::getline(std::cin, line);

        process_buffer(line);
    } else {
        process_from_file(argv[1]);
    }
}
