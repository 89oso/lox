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
    explicit UnaryExpr(TokenType op, Node::ptr expr);
    void accept(Visitor* visitor) override;

    TokenType op;
    Node::ptr expr;
};

struct BinaryExpr : Expr {
    explicit BinaryExpr(TokenType op, Node::ptr left, Node::ptr right);
    void accept(Visitor* visitor) override;

    TokenType op;
    Node::ptr left;
    Node::ptr right;
};

struct GroupingExpr : Expr {
    explicit GroupingExpr(Node::ptr expr);
    void accept(Visitor* visitor) override;

    Node::ptr expr;
};

struct LiteralExpr : Expr {
    enum class Type { Nil, Boolean, Number, String };

    explicit LiteralExpr(Type type);
    void accept(Visitor* visitor) override;

    Type type;

    union {
        bool boolean;
        double number;
        const char* string;
    } value;
};

struct CommaExpr : Expr {
    explicit CommaExpr(std::vector<Node::ptr> expressions);
    void accept(Visitor* visitor) override;

    std::vector<Node::ptr> expressions;
};

struct LogicalExpr : Expr {
    explicit LogicalExpr(TokenType op, Node::ptr left, Node::ptr right);
    void accept(Visitor* visitor) override;

    TokenType op;
    Node::ptr left;
    Node::ptr right;
};

struct ConditionalExpr : Expr {
    explicit ConditionalExpr(Node::ptr expr, Node::ptr left, Node::ptr right);
    void accept(Visitor* visitor) override;

    Node::ptr expr;
    Node::ptr left;
    Node::ptr right;
};
