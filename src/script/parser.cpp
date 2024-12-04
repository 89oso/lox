#include "script/parser.hpp"
#include "script/ast/expr.hpp"

#include "common/exception.hpp"

#include <iostream>

Parser::Parser(const std::string& buffer)
    : _previous(TT_INVALID),
      _error{ false } {
    _lexer = std::make_unique<Lexer>(buffer);
    _current = _lexer->next();
}

Node::ptr Parser::parse() {
    try {
        return parse_expr();
    } catch (const std::exception& e) {
        std::cerr << e.what() << '\n';
        return 0;
    }
}

bool Parser::error() const {
    return _error;
}

Token Parser::advance() {
    _previous = _current;
    _current = _lexer->next();
    return _previous;
}

bool Parser::check(TokenType type) {
    return _current.type == type;
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

    Parser::throw_error(_current, error);
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

    while (_current.type != TokenType::TT_EOF) {
        if (_previous.type == TokenType::TT_SEMICOLON)
            return;

        switch (_current.type) {
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

Node::ptr Parser::parse_expr() {
    return parse_comma_expr();
}

Node::ptr Parser::parse_comma_expr() {
    Node::ptr expr = parse_logical_expr();

    if (check(TokenType::TT_COMMA)) {
        std::vector<Node::ptr> expressions;
        expressions.push_back(std::move(expr));

        while (match(TokenType::TT_COMMA)) {
            Node::ptr next = parse_logical_expr();
            expressions.push_back(std::move(next));
        }

        expr = std::make_unique<CommaExpr>(std::move(expressions));
    }

    return expr;
}

Node::ptr Parser::parse_logical_expr() {
    Node::ptr expr = parse_equality_expr();

    while (matches_any_of(TokenType::TT_AND, TokenType::TT_OR)) {
        Token op = _previous;
        op.print();
        Node::ptr right = parse_equality_expr();

        expr = std::make_unique<LogicalExpr>(op.type, std::move(expr), std::move(right));
    }

    return expr;
}

Node::ptr Parser::parse_equality_expr() {
    Node::ptr expr = parse_comparison_expr();

    while (matches_any_of(TokenType::TT_BANG_EQUAL, TokenType::TT_EQUAL_EQUAL)) {
        Token op = _previous;
        Node::ptr right = parse_comparison_expr();

        expr = std::make_unique<BinaryExpr>(op.type, std::move(expr), std::move(right));
    }

    return expr;
}

Node::ptr Parser::parse_comparison_expr() {
    Node::ptr expr = parse_term_expr();

    while (matches_any_of(
        TokenType::TT_GREATER, TokenType::TT_GREATER_EQUAL, TokenType::TT_LESS, TokenType::TT_LESS_EQUAL)) {
        Token op = _previous;
        Node::ptr right = parse_term_expr();

        expr = std::make_unique<BinaryExpr>(op.type, std::move(expr), std::move(right));
    }

    return expr;
}

Node::ptr Parser::parse_term_expr() {
    Node::ptr expr = parse_factor_expr();

    while (matches_any_of(TokenType::TT_MINUS, TokenType::TT_PLUS)) {
        Token op = _previous;
        Node::ptr right = parse_factor_expr();

        expr = std::make_unique<BinaryExpr>(op.type, std::move(expr), std::move(right));
    }

    return expr;
}

Node::ptr Parser::parse_factor_expr() {
    Node::ptr expr = parse_unary_expr();

    while (matches_any_of(TokenType::TT_SLASH, TokenType::TT_STAR)) {
        Token op = _previous;
        Node::ptr right = parse_unary_expr();

        expr = std::make_unique<BinaryExpr>(op.type, std::move(expr), std::move(right));
    }

    return expr;
}

Node::ptr Parser::parse_unary_expr() {
    if (matches_any_of(TokenType::TT_BANG, TokenType::TT_MINUS)) {
        Token op = _previous;
        Node::ptr expr = parse_unary_expr();

        return std::make_unique<UnaryExpr>(op.type, std::move(expr));
    }

    return parse_primary_expr();
}

Node::ptr Parser::parse_primary_expr() {
    if (match(TokenType::TT_FALSE))
        return std::make_unique<LiteralExpr>("false");
    else if (match(TokenType::TT_TRUE))
        return std::make_unique<LiteralExpr>("true");
    else if (match(TokenType::TT_NIL))
        return std::make_unique<LiteralExpr>("nil");

    if (matches_any_of(TokenType::TT_NUMBER, TokenType::TT_STRING)) {
        Token prev = _previous;
        return std::make_unique<LiteralExpr>(prev.value);
    }

    if (match(TokenType::TT_LEFT_PAREN)) {
        Node::ptr expr = parse_expr();
        consume(TokenType::TT_RIGHT_PAREN, "Expect ')' after expression.");
        return std::make_unique<GroupingExpr>(std::move(expr));
    }

    Parser::throw_error(_current, "Expect expression");
    return 0;
}
