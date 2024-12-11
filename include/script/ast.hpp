#pragma once

#include <memory>
#include <vector>

#include "token.hpp"

struct UnaryExpr;
struct BinaryExpr;
struct GroupingExpr;
struct LiteralExpr;
struct CommaExpr;
struct LogicalExpr;
struct ConditionalExpr;

struct Visitor {
    virtual void visit(UnaryExpr* node) = 0;
    virtual void visit(BinaryExpr* node) = 0;
    virtual void visit(GroupingExpr* node) = 0;
    virtual void visit(LiteralExpr* node) = 0;
    virtual void visit(CommaExpr* node) = 0;
    virtual void visit(LogicalExpr* node) = 0;
    virtual void visit(ConditionalExpr* node) = 0;
};

struct Node {
    using ptr = std::unique_ptr<Node>;

    virtual ~Node() = default;
    virtual void accept(Visitor* visitor) = 0;
};

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
    enum Type {
        Nil,
        Boolean,
        Number,
        String
    };

    explicit LiteralExpr(Type type)
        : type(type) {
    }

    void accept(Visitor* visitor) override {
        visitor->visit(this);
    }

    Type type;

    union {
        bool boolean;
        double number;
        const char* string;
    };
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
