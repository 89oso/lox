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
    i32 _line;
    std::string _message;
};

class ResolverError : public std::exception {
public:
    ResolverError(const std::string& message)
        : _message(message) {
    }

    const char* what() const noexcept override {
        return _message.data();
    }

private:
    i32 _line;
    std::string _message;
};

class RuntimeError : public std::exception {
public:
    RuntimeError(const Token& token, const std::string& message)
        : _token(token),
          _message(message) {
    }

    const Token& token() {
        return _token;
    }

    const char* what() const noexcept override {
        return _message.data();
    }

private:
    Token _token;
    std::string _message;
};
