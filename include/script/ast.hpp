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

class Visitor {
public:
    virtual void visit_unary_expr(UnaryExpr* node) = 0;
    virtual void visit_binary_expr(BinaryExpr* node) = 0;
    virtual void visit_grouping_expr(GroupingExpr* node) = 0;
    virtual void visit_literal_expr(LiteralExpr* node) = 0;
    virtual void visit_comma_expr(CommaExpr* node) = 0;
    virtual void visit_logical_expr(LogicalExpr* node) = 0;
    virtual void visit_conditional_expr(ConditionalExpr* node) = 0;
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

    union Value {
        bool boolean;
        double number;
        std::string string;

        Value() {
        }

        ~Value() {
        }
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
