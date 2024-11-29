#pragma once

#include "../token.hpp"
#include "node.hpp"

struct Expr : public Node {
    virtual ~Expr() = default;
    virtual void accept(Visitor* visitor) = 0;
};

struct UnaryExpr : Expr {
    TokenType op;
    Node::ptr expr;

    UnaryExpr() = default;

    UnaryExpr(TokenType op, Node::ptr expr)
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

    BinaryExpr() = default;

    BinaryExpr(TokenType op, Node::ptr left, Node::ptr right)
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

    GroupingExpr() = default;

    GroupingExpr(Node::ptr expr)
        : expr(std::move(expr)) {
    }

    void accept(Visitor* visitor) override {
        visitor->visit(this);
    }
};

struct LiteralExpr : Expr {
    std::string value;

    LiteralExpr() = default;

    LiteralExpr(std::string value)
        : value(value) {
    }

    void accept(Visitor* visitor) override {
        visitor->visit(this);
    }
};
