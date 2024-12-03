#include "script/parser.hpp"

#include <iostream>

Parser::Parser(std::vector<Token>& tokens)
    : _tokens{ tokens },
      _current{ 0 },
      _error{ false } {
}

Node::ptr Parser::parse() {
    try {
        return expression();
    } catch (const std::exception& e) {
        std::cerr << e.what() << '\n';
        return 0;
    }
}

bool Parser::error() const {
    return _error;
}

bool Parser::is_eof() const {
    return peek().type == TokenType::TT_EOF;
}

Token Parser::advance() {
    if (!is_eof())
        _current++;
    return previous();
}

Token Parser::peek() const {
    return _tokens.at(_current);
}

Token Parser::previous() {
    return _tokens.at(_current - 1);
}

bool Parser::check(TokenType type) {
    if (is_eof())
        return false;
    return peek().type == type;
}

bool Parser::match(TokenType type) {
    if (check(type)) {
        advance();
        return true;
    }

    return false;
}

Token Parser::consume(TokenType type, const char* error) {
    if (check(type))
        return advance();

    Parser::throw_error(peek(), error);
    return Token();
}

void Parser::throw_error(Token token, const std::string& error) {
    _error = true;

    std::string message;
    if (token.type == TokenType::TT_EOF)
        message = "Parser: " + error + " at end (line " + std::to_string(token.line) + ")";
    else
        message = "Parser: " + error + " at '" + token.to_string() + "' (line " + std::to_string(token.line) + ")";

    throw ParseError(message);
}

void Parser::sync() {
    advance();

    while (!is_eof()) {
        if (previous().type == TokenType::TT_SEMICOLON)
            return;

        switch (peek().type) {
        case TokenType::TT_CLASS:
        case TokenType::TT_FUN:
        case TokenType::TT_VAR:
        case TokenType::TT_FOR:
        case TokenType::TT_IF:
        case TokenType::TT_WHILE:
        case TokenType::TT_PRINT:
        case TokenType::TT_RETURN:
            return;
        default:
            break;
        }

        advance();
    }
}

Node::ptr Parser::expression() {
    return equality();
}

Node::ptr Parser::equality() {
    Node::ptr expr = comparison();

    while (matches_any_of(TokenType::TT_BANG_EQUAL, TokenType::TT_EQUAL_EQUAL)) {
        Token op = previous();
        Node::ptr right = comparison();

        expr = std::make_unique<BinaryExpr>(op.type, std::move(expr), std::move(right));
    }

    return expr;
}

Node::ptr Parser::comparison() {
    Node::ptr expr = term();

    while (matches_any_of(
        TokenType::TT_GREATER, TokenType::TT_GREATER_EQUAL, TokenType::TT_LESS, TokenType::TT_LESS_EQUAL)) {
        Token op = previous();
        Node::ptr right = term();

        expr = std::make_unique<BinaryExpr>(op.type, std::move(expr), std::move(right));
    }

    return expr;
}

Node::ptr Parser::term() {
    Node::ptr expr = factor();

    while (matches_any_of(TokenType::TT_MINUS, TokenType::TT_PLUS)) {
        Token op = previous();
        Node::ptr right = factor();

        expr = std::make_unique<BinaryExpr>(op.type, std::move(expr), std::move(right));
    }

    return expr;
}

Node::ptr Parser::factor() {
    Node::ptr expr = unary();

    while (matches_any_of(TokenType::TT_SLASH, TokenType::TT_STAR)) {
        Token op = previous();
        Node::ptr right = unary();

        expr = std::make_unique<BinaryExpr>(op.type, std::move(expr), std::move(right));
    }

    return expr;
}

Node::ptr Parser::unary() {
    if (matches_any_of(TokenType::TT_BANG, TokenType::TT_MINUS)) {
        Token op = previous();
        Node::ptr expr = unary();

        return std::make_unique<UnaryExpr>(op.type, std::move(expr));
    }

    return primary();
}

Node::ptr Parser::primary() {
    if (match(TokenType::TT_FALSE))
        return std::make_unique<LiteralExpr>("false");
    else if (match(TokenType::TT_TRUE))
        return std::make_unique<LiteralExpr>("true");
    else if (match(TokenType::TT_NIL))
        return std::make_unique<LiteralExpr>("nil");

    if (matches_any_of(TokenType::TT_NUMBER, TokenType::TT_STRING)) {
        Token prev = previous();
        return std::make_unique<LiteralExpr>(prev.value);
    }

    if (match(TokenType::TT_LEFT_PAREN)) {
        Node::ptr expr = expression();
        consume(TokenType::TT_RIGHT_PAREN, "Expect ')' after expression.");
        return std::make_unique<GroupingExpr>(std::move(expr));
    }

    Parser::throw_error(peek(), "Expect expression");
    return 0;
}
