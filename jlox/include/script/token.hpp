#pragma once

#include "../common/common.hpp"

#include <string>

enum TokenType : u32 {
    TT_INVALID,
    TT_EOF,

    // Single-character tokens.
    TT_LEFT_PAREN,
    TT_RIGHT_PAREN,
    TT_LEFT_BRACE,
    TT_RIGHT_BRACE,
    TT_COMMA,
    TT_DOT,
    TT_MINUS,
    TT_PLUS,
    TT_SEMICOLON,
    TT_SLASH,
    TT_STAR,
    TT_COLON,
    TT_QMARK,

    // One or two character tokens.
    TT_BANG,
    TT_BANG_EQUAL,
    TT_EQUAL,
    TT_EQUAL_EQUAL,
    TT_GREATER,
    TT_GREATER_EQUAL,
    TT_LESS,
    TT_LESS_EQUAL,

    // Literals.
    TT_IDENTIFIER,
    TT_STRING,
    TT_NUMBER,

    // Keywords.
    TT_RESERVED_BEGIN,
    TT_AND = TT_RESERVED_BEGIN,
    TT_CLASS,
    TT_ELSE,
    TT_FALSE,
    TT_FUN,
    TT_FOR,
    TT_IF,
    TT_NIL,
    TT_OR,
    TT_PRINT,
    TT_RETURN,
    TT_SUPER,
    TT_THIS,
    TT_TRUE,
    TT_VAR,
    TT_WHILE,
    TT_BREAK,
    TT_RESERVED_END,

    TT_RESERVED_COUNT = TT_RESERVED_END - TT_RESERVED_BEGIN,
};

struct Token {
    explicit Token();
    explicit Token(TokenType type);
    explicit Token(TokenType type, u32 line);
    explicit Token(TokenType type, u32 line, const std::string& value);

    Token& operator=(const Token& rhs) {
        type = rhs.type;
        line = rhs.line;
        value = rhs.value;
        return *this;
    }

    u32 type;
    u32 line;
    std::string value;

    static const char* reserved_keywords[];

    std::string to_string();

    void print();
};
