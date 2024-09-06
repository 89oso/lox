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

    void accept(Visitor* visitor) override {
        visitor->visit(this);
    }
};

struct BinaryExpr : Expr {
    TokenType op;
    Expr* left;
    Expr* right;

    void accept(Visitor* visitor) override {
        visitor->visit(this);
    }
};

struct GroupingExpr : Expr {
    Expr* expression;

    void accept(Visitor* visitor) override {
        visitor->visit(this);
    }
};

struct LiteralExpr : Expr {
    std::string value;

    LiteralExpr(std::string value)
        : value(value) {
    }

    void accept(Visitor* visitor) override {
        visitor->visit(this);
    }
};
