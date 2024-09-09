#pragma once

#include "../token.hpp"
#include "node.hpp"

struct Expr : public Node {
    virtual ~Expr() = default;
    virtual void accept(Visitor* visitor) = 0;
};

struct UnaryExpr : Expr {
    TokenType op;
    Expr* right;

    UnaryExpr() = default;

    UnaryExpr(TokenType op, Expr* right)
        : op(op),
          right(right) {
    }

    void accept(Visitor* visitor) override {
        visitor->visit(this);
    }
};

struct BinaryExpr : Expr {
    Expr* left;
    Expr* right;
    TokenType op;

    BinaryExpr() = default;

    BinaryExpr(Expr* left, Expr* right, TokenType op)
        : left(left),
          right(right),
          op(op) {
    }

    void accept(Visitor* visitor) override {
        visitor->visit(this);
    }
};

struct GroupingExpr : Expr {
    Expr* expression;

    GroupingExpr() = default;

    GroupingExpr(Expr* expression)
        : expression(expression) {
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
