#pragma once

#include "token.hpp"
#include "ast/expr.hpp"

#include <stdexcept>
#include <vector>

class ParseError : public std::exception {
public:
    ParseError(const std::string& message)
        : _message(message) {
    }

    const char* what() const noexcept override {
        return _message.data();
    }

private:
    std::string _message;
};

class Parser {
public:
    Parser(std::vector<Token>& tokens);

    Node::ptr parse();
    bool error() const;

private:
    std::vector<Token> _tokens;
    u32 _current;
    bool _error;

    bool is_eof() const;

    Token advance();
    Token peek() const;
    Token previous();

    bool check(TokenType type);
    bool match(TokenType type);
    Token consume(TokenType type, const char* error);

    void throw_error(Token token, const std::string& error);

    void sync();

    template<typename... TokenType>
    bool matches_any_of(TokenType&&... types) {
        if ((check(std::forward<TokenType>(types)) || ...)) {
            advance();
            return true;
        }

        return false;
    }

    Node::ptr expression();
    Node::ptr equality();
    Node::ptr comparison();
    Node::ptr term();
    Node::ptr factor();
    Node::ptr unary();
    Node::ptr primary();
};
