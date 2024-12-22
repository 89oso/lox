#pragma once

#include "lexer.hpp"
#include "ast.hpp"

class Parser {
public:
    Parser(const std::string& buffer);

    std::vector<Stmt::ptr>& parse();
    bool error() const;

private:
    Lexer::ptr _lexer;
    Token _current;
    Token _previous;
    bool _error;
    bool _allow_break_stmt;
    std::vector<Stmt::ptr> _statements;

    Token advance();

    bool check(TokenType type);
    bool match(TokenType type);
    Token consume(TokenType type, const std::string& error);

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

    Stmt::ptr parse_decl();
    Stmt::ptr parse_var_decl();

    Stmt::ptr parse_stmt();
    Stmt::ptr parse_print_stmt();
    Stmt::ptr parse_expr_stmt();
    Stmt::ptr parse_if_stmt();
    Stmt::ptr parse_while_stmt();
    Stmt::ptr parse_for_stmt();
    Stmt::ptr parse_break_stmt();
    Stmt::ptr parse_function_stmt(const std::string& kind);

    std::vector<Node::ptr> parse_block();

    Node::ptr parse_expr();
    Node::ptr parse_assignment_expr();
    Node::ptr parse_conditional_expr();
    Node::ptr parse_logical_or_expr();
    Node::ptr parse_logical_and_expr();
    Node::ptr parse_equality_expr();
    Node::ptr parse_comparison_expr();
    Node::ptr parse_term_expr();
    Node::ptr parse_factor_expr();
    Node::ptr parse_unary_expr();
    Node::ptr parse_call_expr();
    Node::ptr parse_call_expr_arguments(Node::ptr callee);
    Node::ptr parse_primary_expr();
};
