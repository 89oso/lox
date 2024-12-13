#pragma once

#include "ast.hpp"
#include "script_variable.hpp"

class Interpreter : public Visitor {
public:
    Interpreter();

    void visit_unary_expr(UnaryExpr* node) override;
    void visit_binary_expr(BinaryExpr* node) override;
    void visit_grouping_expr(GroupingExpr* node) override;
    void visit_literal_expr(LiteralExpr* node) override;
    void visit_comma_expr(CommaExpr* node) override;
    void visit_logical_expr(LogicalExpr* node) override;
    void visit_conditional_expr(ConditionalExpr* node) override;

    void interpret(Node* node);

private:
    std::vector<ScriptVariable> variable_stack;

    void print_stack();

    bool is_true(ScriptVariable& variable);
    bool is_equal(ScriptVariable& a, ScriptVariable& b);

    void assert_variable_type(Token op, ScriptVariableType type, ScriptVariable& variable);
    void assert_variables_type(Token op, ScriptVariableType type, ScriptVariable& a, ScriptVariable& b);
};
