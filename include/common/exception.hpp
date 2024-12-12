#pragma once

#include <exception>
#include <string>

#include "script/token.hpp"

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

class RuntimeError : public std::exception {
public:
    RuntimeError(Token token, const std::string& message)
        : _token(token),
          _message(message) {
    }

    const char* what() const noexcept override {
        return _message.data();
    }

private:
    Token _token;
    std::string _message;
};
