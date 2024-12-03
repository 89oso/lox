#pragma once

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
