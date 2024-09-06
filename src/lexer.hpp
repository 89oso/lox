#pragma once

#include "token.hpp"
#include <string>
#include <vector>

class Lexer {
public:
    Lexer(const std::string& buffer);

    void tokenize();

private:
    std::string _buffer;
    size_t _buffer_size;
    std::vector<Token> _tokens;

    bool _error;
    int _start;
    int _current;
    int _line;
    int _column;

    void add_token(TokenType type);
    void add_token(TokenType type, std::string value);
    void error(const std::string& msg);
    void print_tokens();
    void scan();

    bool is_digit(char c) const;
    bool is_alpha(char c) const;
    bool is_alphanumeric(char c) const;
    bool is_eof() const;

    char advance();
    char peek();
    char peek_next();

    void string();
    void number();
    void identifier();
};
