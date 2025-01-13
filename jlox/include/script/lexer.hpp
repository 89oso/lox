#pragma once

#include "token.hpp"
#include <string>

namespace script {

class Lexer {
public:
    using ptr = std::unique_ptr<Lexer>;

    Lexer(const std::string& buffer);

    void print();
    Token next();

private:
    std::string _buffer;
    usize _buffer_size;

    bool _error;
    u32 _start;
    u32 _current;
    u32 _line;
    u32 _column;

    void error(const std::string& msg);

    bool is_digit(char c) const;
    bool is_alpha(char c) const;
    bool is_alphanumeric(char c) const;
    bool is_eof() const;

    char advance();
    char peek();
    char peek_next();

    Token string();
    Token number();
    Token identifier();
};

} // namespace script
