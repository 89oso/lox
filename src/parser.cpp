#include "parser.hpp"

Parser::Parser(std::vector<Token>& tokens)
    : _tokens{ tokens },
      _current{ 0 } {
}

bool Parser::is_eof() const {
    return peek().type != TokenType::TT_EOF;
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

Expr* Parser::expression() {
    return equality();
}

// equality: comparison ( ( "!=" | "==" ) comparison )* ;
Expr* Parser::equality() {
    Expr* expr = comparison();

    while (matches_any_of(TokenType::TT_BANG_EQUAL, TokenType::TT_EQUAL_EQUAL)) {
        Token op = previous();
        Expr* right = comparison();

        expr = new BinaryExpr(expr, right, op.type);
    }

    return expr;
}

// comparison: term ( ( ">" | ">=" | "<" | "<=" ) term )* ;
Expr* Parser::comparison() {
    Expr* expr = term();

    while (matches_any_of(
        TokenType::TT_GREATER, TokenType::TT_GREATER_EQUAL, TokenType::TT_LESS, TokenType::TT_LESS_EQUAL)) {
        Token op = previous();
        Expr* right = term();

        expr = new BinaryExpr(expr, right, op.type);
    }

    return expr;
}

Expr* Parser::term() {
}
