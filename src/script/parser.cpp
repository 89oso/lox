#include "script/parser.hpp"

#include "common/exception.hpp"

#include <iostream>

Parser::Parser(const std::string& buffer)
    : _previous(TT_INVALID),
      _error(false),
      _allow_break_stmt(false) {
    _lexer = std::make_unique<Lexer>(buffer);
    _current = _lexer->next();
}

std::vector<Stmt::ptr>& Parser::parse() {
    while (_current.type != TokenType::TT_EOF) {
        auto stmt = parse_decl();
        _statements.push_back(std::move(stmt));
    }

    return _statements;
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
        message = error + " at EOF (line " + std::to_string(token.line) + ")";
    else
        message = error + " at '" + token.to_string() + "' (line " + std::to_string(token.line) + ")";

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

Stmt::ptr Parser::parse_decl() {
    try {
        if (match(TokenType::TT_VAR))
            return parse_var_decl();

        return parse_stmt();
    } catch (const ParseError& e) {
        std::cerr << "[parser error]: " << e.what() << "\n";
        sync();
        return nullptr;
    }
}

Stmt::ptr Parser::parse_var_decl() {
    Token name = consume(TokenType::TT_IDENTIFIER, "Expect variabled name");

    Expr::ptr initializer = nullptr;
    if (match(TokenType::TT_EQUAL)) {
        initializer = parse_expr();
    }

    consume(TokenType::TT_SEMICOLON, "Expect ';' after expression");

    return std::make_unique<VarStmt>(name, std::move(initializer));
}

Stmt::ptr Parser::parse_stmt() {
    if (match(TokenType::TT_PRINT))
        return parse_print_stmt();
    else if (match(TokenType::TT_LEFT_BRACE))
        return std::make_unique<BlockStmt>(parse_block());
    else if (match(TokenType::TT_IF))
        return parse_if_stmt();
    else if (match(TokenType::TT_WHILE))
        return parse_while_stmt();
    else if (match(TokenType::TT_FOR))
        return parse_for_stmt();
    else if (match(TokenType::TT_BREAK))
        return parse_break_stmt();

    return parse_expr_stmt();
}

Stmt::ptr Parser::parse_print_stmt() {
    auto expr = parse_expr();
    consume(TokenType::TT_SEMICOLON, "Expect ';' after expression");

    return std::make_unique<PrintStmt>(std::move(expr));
}

Stmt::ptr Parser::parse_expr_stmt() {
    auto expr = parse_expr();
    consume(TokenType::TT_SEMICOLON, "Expect ';' after expression");

    return std::make_unique<ExprStmt>(std::move(expr));
}

Stmt::ptr Parser::parse_if_stmt() {
    consume(TokenType::TT_LEFT_PAREN, "Expect '(' after 'if'");
    Node::ptr condition = parse_expr();
    consume(TokenType::TT_RIGHT_PAREN, "Expect ')' after condition");

    Stmt::ptr then_branch = parse_stmt();
    Stmt::ptr else_branch = match(TokenType::TT_ELSE) ? parse_stmt() : nullptr;

    return std::make_unique<IfStmt>(std::move(condition), std::move(then_branch), std::move(else_branch));
}

Stmt::ptr Parser::parse_while_stmt() {
    consume(TokenType::TT_LEFT_PAREN, "Expect '(' after 'while'");
    Node::ptr condition = parse_expr();
    consume(TokenType::TT_RIGHT_PAREN, "Expect ')' after while condition");

    _allow_break_stmt = true;
    Stmt::ptr body = parse_stmt();
    _allow_break_stmt = false;

    return std::make_unique<WhileStmt>(std::move(condition), std::move(body));
}

Stmt::ptr Parser::parse_for_stmt() {
    consume(TokenType::TT_LEFT_PAREN, "Expect '(' after 'for'");

    Stmt::ptr initializer = nullptr;
    if (match(TokenType::TT_SEMICOLON))
        ;
    else if (match(TokenType::TT_VAR))
        initializer = parse_var_decl();
    else
        initializer = parse_expr_stmt();

    Node::ptr condition = nullptr;
    if (!check(TokenType::TT_SEMICOLON))
        condition = parse_expr();

    consume(TokenType::TT_SEMICOLON, "Expect ';' after loop condition");

    Node::ptr increment = nullptr;
    if (!check(TokenType::TT_RIGHT_PAREN))
        increment = parse_expr();

    consume(TokenType::TT_RIGHT_PAREN, "Expect ')' after for clauses");

    _allow_break_stmt = true;
    Stmt::ptr body = parse_stmt();
    _allow_break_stmt = false;

    // if we have an increment expr then we can 'append' it to the body stmt
    if (increment) {
        std::vector<Node::ptr> statements;
        statements.push_back(std::move(body));
        statements.push_back(std::move(increment));
        body = std::make_unique<BlockStmt>(std::move(statements));
    }

    // if the condition was omitted then we emplace a 'true' literal expr
    if (!condition) {
        condition = std::make_unique<LiteralExpr>(LiteralExpr::LiteralType::Boolean);
        LiteralExpr* literal_expr = reinterpret_cast<LiteralExpr*>(condition.get());
        literal_expr->value.boolean = true;
    }

    body = std::make_unique<WhileStmt>(std::move(condition), std::move(body));

    // if we have an initializer node then we can 'prepend' to the body stmt
    if (initializer) {
        std::vector<Node::ptr> statements;
        statements.push_back(std::move(initializer));
        statements.push_back(std::move(body));
        body = std::make_unique<BlockStmt>(std::move(statements));
    }

    return body;
}

Stmt::ptr Parser::parse_break_stmt() {
    if (_allow_break_stmt) {
        consume(TokenType::TT_SEMICOLON, "Expect ';' after 'break'");
        return std::make_unique<BreakStmt>();
    }

    throw_error(_current, "a break statement may only be used within a loop");
    return nullptr;
}

std::vector<Node::ptr> Parser::parse_block() {
    std::vector<Node::ptr> statements;

    while (!check(TokenType::TT_RIGHT_BRACE) && _current.type != TokenType::TT_EOF)
        statements.push_back(std::move(parse_decl()));

    consume(TokenType::TT_RIGHT_BRACE, "Expect '}' after block");

    return statements;
}

Node::ptr Parser::parse_expr() {
    return parse_assignment_expr();
}

Node::ptr Parser::parse_assignment_expr() {
    auto expr = parse_comma_expr();

    if (match(TokenType::TT_EQUAL)) {
        Token equals = _previous;
        Node::ptr value = parse_assignment_expr();

        if (expr->type() == Node::Type::VariableExpr) {
            Token name = reinterpret_cast<VariableExpr*>(expr.get())->name;
            return std::make_unique<AssignmentExpr>(name, std::move(value));
        }

        throw_error(equals, "Invalid assignment target");
    }

    return expr;
}

Node::ptr Parser::parse_comma_expr() {
    Node::ptr expr = parse_conditional_expr();

    if (check(TokenType::TT_COMMA)) {
        std::vector<Node::ptr> expressions;
        expressions.push_back(std::move(expr));

        while (match(TokenType::TT_COMMA)) {
            Node::ptr next = parse_conditional_expr();
            expressions.push_back(std::move(next));
        }

        expr = std::make_unique<CommaExpr>(std::move(expressions));
    }

    return expr;
}

Node::ptr Parser::parse_conditional_expr() {
    Node::ptr expr = parse_logical_or_expr();

    while (match(TokenType::TT_QMARK)) {
        Node::ptr left = parse_expr();
        consume(TokenType::TT_COLON, "Expect ':' after expression.");
        Node::ptr right = parse_expr();

        expr = std::make_unique<ConditionalExpr>(std::move(expr), std::move(left), std::move(right));
    }

    return expr;
}

Node::ptr Parser::parse_logical_or_expr() {
    Node::ptr expr = parse_logical_and_expr();

    while (match(TokenType::TT_OR)) {
        Token op = _previous;
        Node::ptr right = parse_logical_and_expr();

        expr = std::make_unique<LogicalExpr>(op, std::move(expr), std::move(right));
    }

    return expr;
}

Node::ptr Parser::parse_logical_and_expr() {
    Node::ptr expr = parse_equality_expr();

    while (match(TokenType::TT_AND)) {
        Token op = _previous;
        Node::ptr right = parse_equality_expr();

        expr = std::make_unique<LogicalExpr>(op, std::move(expr), std::move(right));
    }

    return expr;
}

Node::ptr Parser::parse_equality_expr() {
    Node::ptr expr = parse_comparison_expr();

    while (matches_any_of(TokenType::TT_BANG_EQUAL, TokenType::TT_EQUAL_EQUAL)) {
        Token op = _previous;
        Node::ptr right = parse_comparison_expr();

        expr = std::make_unique<BinaryExpr>(op, std::move(expr), std::move(right));
    }

    return expr;
}

Node::ptr Parser::parse_comparison_expr() {
    Node::ptr expr = parse_term_expr();

    while (matches_any_of(
        TokenType::TT_GREATER, TokenType::TT_GREATER_EQUAL, TokenType::TT_LESS, TokenType::TT_LESS_EQUAL)) {
        Token op = _previous;
        Node::ptr right = parse_term_expr();

        expr = std::make_unique<BinaryExpr>(op, std::move(expr), std::move(right));
    }

    return expr;
}

Node::ptr Parser::parse_term_expr() {
    Node::ptr expr = parse_factor_expr();

    while (matches_any_of(TokenType::TT_MINUS, TokenType::TT_PLUS)) {
        Token op = _previous;
        Node::ptr right = parse_factor_expr();

        expr = std::make_unique<BinaryExpr>(op, std::move(expr), std::move(right));
    }

    return expr;
}

Node::ptr Parser::parse_factor_expr() {
    Node::ptr expr = parse_unary_expr();

    while (matches_any_of(TokenType::TT_SLASH, TokenType::TT_STAR)) {
        Token op = _previous;
        Node::ptr right = parse_unary_expr();

        expr = std::make_unique<BinaryExpr>(op, std::move(expr), std::move(right));
    }

    return expr;
}

Node::ptr Parser::parse_unary_expr() {
    if (matches_any_of(TokenType::TT_BANG, TokenType::TT_MINUS)) {
        Token op = _previous;
        Node::ptr expr = parse_unary_expr();

        return std::make_unique<UnaryExpr>(op, std::move(expr));
    }

    return parse_primary_expr();
}

Node::ptr Parser::parse_primary_expr() {
    if (match(TokenType::TT_FALSE)) {
        auto node = std::make_unique<LiteralExpr>(LiteralExpr::LiteralType::Boolean);
        node->value.boolean = false;
        return node;
    } else if (match(TokenType::TT_TRUE)) {
        auto node = std::make_unique<LiteralExpr>(LiteralExpr::LiteralType::Boolean);
        node->value.boolean = true;
        return node;
    } else if (match(TokenType::TT_NIL)) {
        return std::make_unique<LiteralExpr>(LiteralExpr::LiteralType::Nil);
    } else if (match(TokenType::TT_NUMBER)) {
        auto node = std::make_unique<LiteralExpr>(LiteralExpr::LiteralType::Number);
        node->value.number = std::stod(_previous.value);
        return node;
    } else if (match(TokenType::TT_STRING)) {
        auto node = std::make_unique<LiteralExpr>(LiteralExpr::LiteralType::String);
        node->value.string = _previous.value;
        return node;
    } else if (match(TokenType::TT_IDENTIFIER)) {
        auto node = std::make_unique<VariableExpr>(_previous);
        return node;
    }

    if (match(TokenType::TT_LEFT_PAREN)) {
        Node::ptr expr = parse_expr();
        consume(TokenType::TT_RIGHT_PAREN, "Expect ')' after expression.");
        return std::make_unique<GroupingExpr>(std::move(expr));
    }

    Parser::throw_error(_current, "Expect expression");
    return nullptr;
}
