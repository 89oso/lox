#include "lox/lox.hpp"
#include <fstream>
#include <iostream>

static void process_command_line() {
    std::string line;
    std::getline(std::cin, line);

    lox::process_buffer(line);
}

static void process_script(const char* path) {
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

    lox::process_script(path, buffer);
}

int main(int argc, char** argv) {
    if (argc > 2) {
        std::cerr << "./lox [script]\n";
        return 1;
    }

    if (argc == 1)
        process_command_line();
    else
        process_script(argv[1]);
}
