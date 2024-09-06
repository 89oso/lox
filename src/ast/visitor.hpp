#pragma once

struct UnaryExpr;
struct BinaryExpr;
struct GroupingExpr;
struct LiteralExpr;

struct Visitor {
    virtual ~Visitor() = default;

    virtual void visit(UnaryExpr* expr) = 0;
    virtual void visit(BinaryExpr* expr) = 0;
    virtual void visit(GroupingExpr* expr) = 0;
    virtual void visit(LiteralExpr* expr) = 0;
};
