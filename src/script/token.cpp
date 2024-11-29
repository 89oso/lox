#include "token.hpp"
#include <iostream>

const char* Token::reserved_keywords[] = {
    "and", "class", "else",   "false", "fun",  "for",  "if",  "nil",
    "or",  "print", "return", "super", "this", "true", "var", "while",
};

Token::Token()
    : type(TokenType::TT_EOF),
      line(0) {
}

Token::Token(TokenType type)
    : type(type),
      line(0) {
}

Token::Token(u32 line, TokenType type)
    : line(line),
      type(type) {
}

Token::Token(u32 line, TokenType type, const std::string& value)
    : line(line),
      type(type),
      value(value) {
}

std::string Token::to_string() {
    switch (type) {
    case TokenType::TT_LEFT_PAREN:
        return "(";
    case TokenType::TT_RIGHT_PAREN:
        return ")";
    case TokenType::TT_LEFT_BRACE:
        return "{";
    case TokenType::TT_RIGHT_BRACE:
        return "}";
    case TokenType::TT_COMMA:
        return ",";
    case TokenType::TT_DOT:
        return ".";
    case TokenType::TT_MINUS:
        return "-";
    case TokenType::TT_PLUS:
        return "+";
    case TokenType::TT_SEMICOLON:
        return ";";
    case TokenType::TT_SLASH:
        return "/";
    case TokenType::TT_STAR:
        return "*";
    case TokenType::TT_BANG:
        return "!";
    case TokenType::TT_BANG_EQUAL:
        return "!=";
    case TokenType::TT_EQUAL:
        return "=";
    case TokenType::TT_EQUAL_EQUAL:
        return "==";
    case TokenType::TT_GREATER:
        return ">";
    case TokenType::TT_GREATER_EQUAL:
        return ">=";
    case TokenType::TT_LESS:
        return "<";
    case TokenType::TT_LESS_EQUAL:
        return "<=";
    case TokenType::TT_IDENTIFIER:
        return "identifier";
    case TokenType::TT_STRING:
        return "string";
    case TokenType::TT_NUMBER:
        return "number";
    case TokenType::TT_EOF:
        return "<EOF>";
    default:
        if (type >= TokenType::TT_RESERVED_BEGIN && type < TokenType::TT_RESERVED_END)
            return "keyword";
        // return Token::reserved_keywords[type - TokenType::TT_RESERVED_BEGIN];
        return "UNKNOWN TOKEN";
    }
}

void Token::print() {
    std::cout << to_string();
    if (type == TokenType::TT_STRING || type == TokenType::TT_IDENTIFIER || type == TokenType::TT_NUMBER
        || (type >= TokenType::TT_RESERVED_BEGIN && type < TokenType::TT_RESERVED_END)) {
        std::cout << ": " << value;
    }
    std::cout << "\n";
}
