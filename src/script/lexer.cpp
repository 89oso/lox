#include "script/lexer.hpp"
#include <iostream>

Lexer::Lexer(const std::string& buffer)
    : _buffer(buffer),
      _current(0),
      _start(0),
      _line(1),
      _error(false) {
    _buffer_size = buffer.size();
}

void Lexer::tokenize() {
    while (!is_eof() && !_error) {
        _start = _current;
        scan();
    }

    Token token(_line, TT_EOF);
    _tokens.push_back(token);

    print_tokens();
}

std::vector<Token>& Lexer::tokens() {
    return _tokens;
}

void Lexer::add_token(TokenType type) {
    Token token(_line, type);
    _tokens.push_back(token);
}

void Lexer::add_token(TokenType type, std::string value) {
    Token token(_line, type);
    token.value = value;
    _tokens.push_back(token);
}

void Lexer::error(const std::string& msg) {
    std::cerr << "ERROR: " << msg << "\n";
    _error = true;
}

void Lexer::print_tokens() {
    std::cout << "---- tokens ----\n";
    for (size_t i = 0; i < _tokens.size(); i++) {
        _tokens.at(i).print();
    }
    std::cout << "----------------\n";
}

void Lexer::scan() {
    char c = advance();

    switch (c) {
    case '"':
        string();
        break;
    case ' ':
    case '\r':
    case '\t':
        // Ignore whitespace.
        break;
    case '\n':
        _line++;
        _column = 0;
        break;
    case '/':
        if (peek() == '/') {
            while (peek() != '\n' && !is_eof())
                advance();
        } else {
            add_token(TT_SLASH);
        }
        break;
    case '(':
        add_token(TT_LEFT_PAREN);
        break;
    case ')':
        add_token(TT_RIGHT_PAREN);
        break;
    case '{':
        add_token(TT_LEFT_BRACE);
        break;
    case '}':
        add_token(TT_RIGHT_BRACE);
        break;
    case ',':
        add_token(TT_COMMA);
        break;
    case '.':
        add_token(TT_DOT);
        break;
    case '-':
        add_token(TT_MINUS);
        break;
    case '+':
        add_token(TT_PLUS);
        break;
    case ';':
        add_token(TT_SEMICOLON);
        break;
    case '*':
        add_token(TT_STAR);
        break;
    case '!':
        if (peek() == '=') {
            advance();
            add_token(TT_BANG_EQUAL);
        } else {
            add_token(TT_BANG);
        }
        break;
    case '=':
        if (peek() == '=') {
            advance();
            add_token(TT_EQUAL_EQUAL);
        } else {
            add_token(TT_EQUAL);
        }
        break;
    case '<':
        if (peek() == '=') {
            advance();
            add_token(TT_LESS_EQUAL);
        } else {
            add_token(TT_LESS);
        }
        break;
    case '>':
        if (peek() == '=') {
            advance();
            add_token(TT_GREATER_EQUAL);
        } else {
            add_token(TT_GREATER);
        }
        break;
    default:
        if (is_digit(c)) {
            number();
        } else if (c == '_' || is_alpha(c)) {
            identifier();
        } else {
            error("unexpected character: " + std::string(&c, 1));
        }
        break;
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
    if (is_eof()) {
        return '\0';
    }
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

void Lexer::string() {
    while (peek() != '"' && peek() != '\n' && !is_eof())
        advance();

    if (peek() == '\n' || is_eof()) {
        error("unterminated string");
        return;
    }

    add_token(TT_STRING, _buffer.substr(_start + 1, _current - _start - 1));

    // The closing "
    advance();
}

void Lexer::number() {
    while (is_digit(peek()))
        advance();
    if (peek() == '.') {
        if (!is_digit(peek_next())) {
            error("expected number after decimal point");
            return;
        }

        // consume the '.'
        advance();

        while (is_digit(peek()))
            advance();
    }

    add_token(TT_NUMBER, _buffer.substr(_start, _current - _start));
}

void Lexer::identifier() {
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

    add_token(static_cast<TokenType>(type), name);
}
