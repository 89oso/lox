#pragma once

#include "token.hpp"
#include "ast/expr.hpp"

#include <vector>

class Parser {
public:
    Parser(std::vector<Token>& tokens);

private:
    std::vector<Token> _tokens;
    int _current;

    bool is_eof() const;

    Token advance();
    Token peek() const;
    Token previous();

    bool check(TokenType type);
    bool match(TokenType type);

    template<typename... TokenType>
    bool matches_any_of(TokenType&&... types) {
        if ((check(std::forward<TokenType>(types)) || ...)) {
            advance();
            return true;
        }

        return false;
    }

    Expr* expression();
    Expr* equality();
    Expr* comparison();
    Expr* term();
};
