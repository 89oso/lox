#pragma once

#include "ast.hpp"
#include "script_variable.hpp"

struct Interpreter : public Visitor {
    std::vector<ScriptVariable> variable_stack;

    Interpreter();

    void print_stack();

    void visit(UnaryExpr* node) override;
    void visit(BinaryExpr* node) override;
    void visit(GroupingExpr* node) override;
    void visit(LiteralExpr* node) override;
    void visit(CommaExpr* node) override;
    void visit(LogicalExpr* node) override;
    void visit(ConditionalExpr* node) override;
};
