#include "lox.hpp"
#include "lexer.hpp"
#include <iostream>

namespace lox {
    void process_buffer(const std::string& buffer) {
        Lexer lexer(buffer);
        lexer.tokenize();
    }

    void process_script(const char* path, const std::string& buffer) {
        std::cout << "Processing script: " << path << "\n";

        Lexer lexer(buffer);
        lexer.tokenize();
    }
} // namespace lox
