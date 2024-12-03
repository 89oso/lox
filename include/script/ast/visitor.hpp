#pragma once

struct UnaryExpr;
struct BinaryExpr;
struct GroupingExpr;
struct LiteralExpr;
struct CommaExpr;

struct Visitor {
    virtual ~Visitor() = default;

    virtual void visit(UnaryExpr* node) = 0;
    virtual void visit(BinaryExpr* node) = 0;
    virtual void visit(GroupingExpr* node) = 0;
    virtual void visit(LiteralExpr* node) = 0;
    virtual void visit(CommaExpr* node) = 0;
};
