#pragma once

#include <string>

namespace lox {
    void process_buffer(const std::string& buffer);
    void process_script(const char* path, const std::string& buffer);
} // namespace lox
