#pragma once

#include "lexer.hpp"
#include "ast/node.hpp"

#include <vector>

class Parser {
public:
    Parser(const std::string& buffer);

    Node::ptr parse();
    bool error() const;

private:
    Lexer::ptr _lexer;
    Token _current;
    Token _previous;
    bool _error;

    Token advance();

    bool check(TokenType type);
    bool match(TokenType type);
    Token consume(TokenType type, const char* error);

    void throw_error(Token token, const std::string& error);

    void sync();

    template<typename... TokenType>
    bool matches_any_of(TokenType&&... types) {
        if ((check(std::forward<TokenType>(types)) || ...)) {
            advance();
            return true;
        }

        return false;
    }

    std::vector<Node::ptr> comma_expressions;

    Node::ptr parse_expr();
    Node::ptr parse_comma_expr();
    Node::ptr parse_equality_expr();
    Node::ptr parse_comparison_expr();
    Node::ptr parse_term_expr();
    Node::ptr parse_factor_expr();
    Node::ptr parse_unary_expr();
    Node::ptr parse_primary_expr();
};
