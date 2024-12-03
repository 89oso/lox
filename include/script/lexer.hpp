#pragma once

#include "token.hpp"
#include <string>
#include <vector>

class Lexer {
public:
    Lexer(const std::string& buffer);

    void tokenize();
    std::vector<Token>& tokens();

private:
    std::string _buffer;
    usize _buffer_size;
    std::vector<Token> _tokens;

    bool _error;
    u32 _start;
    u32 _current;
    u32 _line;
    u32 _column;

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
