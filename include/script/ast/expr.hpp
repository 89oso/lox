#pragma once

#include "../token.hpp"
#include "node.hpp"

#include <vector>

struct Expr : public Node {
    virtual ~Expr() = default;
    virtual void accept(Visitor* visitor) = 0;
};

struct UnaryExpr : Expr {
    TokenType op;
    Node::ptr expr;

    explicit UnaryExpr(TokenType op, Node::ptr expr)
        : op(op),
          expr(std::move(expr)) {
    }

    void accept(Visitor* visitor) override {
        visitor->visit(this);
    }
};

struct BinaryExpr : Expr {
    TokenType op;
    Node::ptr left;
    Node::ptr right;

    explicit BinaryExpr(TokenType op, Node::ptr left, Node::ptr right)
        : op(op),
          left(std::move(left)),
          right(std::move(right)) {
    }

    void accept(Visitor* visitor) override {
        visitor->visit(this);
    }
};

struct GroupingExpr : Expr {
    Node::ptr expr;

    explicit GroupingExpr(Node::ptr expr)
        : expr(std::move(expr)) {
    }

    void accept(Visitor* visitor) override {
        visitor->visit(this);
    }
};

struct LiteralExpr : Expr {
    std::string value;

    explicit LiteralExpr(std::string value)
        : value(value) {
    }

    void accept(Visitor* visitor) override {
        visitor->visit(this);
    }
};

struct CommaExpr : Expr {
    std::vector<Node::ptr> expressions;

    explicit CommaExpr(std::vector<Node::ptr> expressions)
        : expressions(std::move(expressions)) {
    }

    void accept(Visitor* visitor) override {
        visitor->visit(this);
    }
};

struct LogicalExpr : Expr {
    TokenType op;
    Node::ptr left;
    Node::ptr right;

    explicit LogicalExpr(TokenType op, Node::ptr left, Node::ptr right)
        : op(op),
          left(std::move(left)),
          right(std::move(right)) {
    }

    void accept(Visitor* visitor) override {
        visitor->visit(this);
    }
};

struct ConditionalExpr : Expr {
    Node::ptr expr;
    Node::ptr left;
    Node::ptr right;

    explicit ConditionalExpr(Node::ptr expr, Node::ptr left, Node::ptr right)
        : expr(std::move(expr)),
          left(std::move(left)),
          right(std::move(right)) {
    }

    void accept(Visitor* visitor) override {
        visitor->visit(this);
    }
};

