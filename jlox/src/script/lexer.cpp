#include "script/lexer.hpp"
#include <iostream>

namespace script {

Lexer::Lexer(const std::string& buffer)
    : _buffer(buffer),
      _current(0),
      _start(0),
      _line(1),
      _error(false) {
    _buffer_size = buffer.size();
}

void Lexer::error(const std::string& msg) {
    _error = true;
    std::cerr << "[lexer error]: " << msg << " at line " << _line << ", col " << _column << "\n";
}

void Lexer::print() {
    std::cout << "---- tokens ----\n";

    Token token = next();
    while (token.type != TT_EOF && !_error) {
        token.print();
        token = next();
    }

    std::cout << "----------------\n";
}

Token Lexer::next() {
    while (true) {
        _start = _current;

        char c = advance();

        switch (c) {
        case ' ':
        case '\r':
        case '\t':
            break;
        case '\n':
            _line++;
            _column = 0;
            break;
        case '/':
            if (peek() == '/') {
                while (peek() != '\n' && !is_eof())
                    advance();
                break;
            }
            return Token(TT_SLASH, _line);
        case '(':
            return Token(TT_LEFT_PAREN, _line);
        case ')':
            return Token(TT_RIGHT_PAREN, _line);
        case '{':
            return Token(TT_LEFT_BRACE, _line);
        case '}':
            return Token(TT_RIGHT_BRACE, _line);
        case ',':
            return Token(TT_COMMA, _line);
        case '.':
            return Token(TT_DOT, _line);
        case '-':
            return Token(TT_MINUS, _line);
        case '+':
            return Token(TT_PLUS, _line);
        case ';':
            return Token(TT_SEMICOLON, _line);
        case '*':
            return Token(TT_STAR, _line);
        case ':':
            return Token(TT_COLON, _line);
        case '?':
            return Token(TT_QMARK, _line);
        case '!':
            if (peek() == '=') {
                advance();
                return Token(TT_BANG_EQUAL, _line);
            }
            return Token(TT_BANG, _line);
        case '=':
            if (peek() == '=') {
                advance();
                return Token(TT_EQUAL_EQUAL, _line);
            }
            return Token(TT_EQUAL, _line);
        case '<':
            if (peek() == '=') {
                advance();
                return Token(TT_LESS_EQUAL, _line);
            }
            return Token(TT_LESS, _line);
        case '>':
            if (peek() == '=') {
                advance();
                return Token(TT_GREATER_EQUAL, _line);
            }
            return Token(TT_GREATER, _line);
        default:
            if (c == '\0')
                return Token(TT_EOF, _line);
            else if (c == '"')
                return string();
            else if (is_digit(c))
                return number();
            else if (c == '_' || is_alpha(c))
                return identifier();
            error("unexpected character: " + std::string(&c, 1));
        }
    }
}

bool Lexer::is_digit(char c) const {
    return (c >= '0' && c <= '9');
}

bool Lexer::is_alpha(char c) const {
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
}

bool Lexer::is_alphanumeric(char c) const {
    return is_alpha(c) || is_digit(c);
}

bool Lexer::is_eof() const {
    return _current >= _buffer_size;
}

char Lexer::advance() {
    if (is_eof())
        return '\0';
    _column++;
    return _buffer.at(_current++);
}

char Lexer::peek() {
    if (is_eof())
        return 0;
    return _buffer.at(_current);
}

char Lexer::peek_next() {
    if (_current + 1 >= _buffer_size || is_eof())
        return 0;
    return _buffer.at(_current + 1);
}

Token Lexer::string() {
    while (peek() != '"' && peek() != '\n' && !is_eof())
        advance();

    if (peek() == '\n' || is_eof()) {
        error("unterminated string");
        return Token(TT_INVALID, _line);
    }

    Token token = Token(TT_STRING, _line, _buffer.substr(_start + 1, _current - _start - 1));

    // The closing quote
    advance();

    return token;
}

Token Lexer::number() {
    while (is_digit(peek()))
        advance();

    if (peek() == '.') {
        if (!is_digit(peek_next())) {
            error("expected number after decimal point");
            return Token(TT_INVALID, _line);
        }

        // consume the '.'
        advance();

        while (is_digit(peek()))
            advance();
    }

    return Token(TT_NUMBER, _line, _buffer.substr(_start, _current - _start));
}

Token Lexer::identifier() {
    while (peek() == '_' || is_alphanumeric(peek()))
        advance();

    u32 type = TT_IDENTIFIER;

    std::string name = _buffer.substr(_start, _current - _start);

    for (u32 i = 0; i < TT_RESERVED_COUNT; i++) {
        if (name == Token::reserved_keywords[i]) {
            type = i + TT_RESERVED_BEGIN;
            break;
        }
    }

    return Token(static_cast<TokenType>(type), _line, name);
}

} // namespace script
