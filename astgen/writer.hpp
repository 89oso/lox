#pragma once

#include <fstream>
#include <string>

class Writer {
public:
    void write(int indent, std::string str) {
        for (auto i = 0; i < indent; i++)
            _buf += "    ";
        _buf += str;
    }

    void write_line(int indent = 0, std::string str = "") {
        for (auto i = 0; i < indent; i++)
            _buf += "    ";
        _buf += str;
        _buf += "\n";
    }

    void flush_to_file(const std::string& filepath) {
        std::ofstream file(filepath.data());

        if (!file.is_open()) {
            return;
        }

        file.write(_buf.data(), _buf.size());

        _buf.clear();
    }

private:
    std::string _buf;
};
