#include "token.hpp"
#include <iostream>

namespace lox {
    const char* Token::reserved_keywords[] = {
        "and", "class", "else",   "false", "fun",  "for",  "if",  "nil",
        "or",  "print", "return", "super", "this", "true", "var", "while",
    };

    Token::Token(TokenType type)
        : type(type) {
        line = 0;
    }

    Token::Token(int line, TokenType type)
        : line(line),
          type(type) {
    }

    Token::Token(int line, int type, const std::string& value)
        : line(line),
          type(type),
          value(value) {
    }

    std::string Token::to_string() {
        switch (type) {
        case TT_LEFT_PAREN:
            return "(";
        case TT_RIGHT_PAREN:
            return ")";
        case TT_LEFT_BRACE:
            return "{";
        case TT_RIGHT_BRACE:
            return "}";
        case TT_COMMA:
            return ",";
        case TT_DOT:
            return ".";
        case TT_MINUS:
            return "-";
        case TT_PLUS:
            return "+";
        case TT_SEMICOLON:
            return ";";
        case TT_SLASH:
            return "/";
        case TT_STAR:
            return "*";
        case TT_BANG:
            return "!";
        case TT_BANG_EQUAL:
            return "!=";
        case TT_EQUAL:
            return "=";
        case TT_EQUAL_EQUAL:
            return "==";
        case TT_GREATER:
            return ">";
        case TT_GREATER_EQUAL:
            return ">=";
        case TT_LESS:
            return "<";
        case TT_LESS_EQUAL:
            return "<=";
        case TT_IDENTIFIER:
            return "identifier";
        case TT_STRING:
            return "string";
        case TT_NUMBER:
            return "number";
        case TT_EOF:
            return "<EOF>";
        default:
            if (type >= TT_RESERVED_BEGIN && type < TT_RESERVED_END)
                return "keyword";
            // return Token::reserved_keywords[type - TT_RESERVED_BEGIN];
            return "UNKNOWN TOKEN";
        }
    }

    void Token::print() {
        std::cout << to_string();
        if (type == TokenType::TT_STRING || type == TokenType::TT_IDENTIFIER || type == TokenType::TT_NUMBER
            || (type >= TT_RESERVED_BEGIN && type < TT_RESERVED_END)) {
            std::cout << ": " << value;
        }
        std::cout << "\n";
    }
} // namespace lox
