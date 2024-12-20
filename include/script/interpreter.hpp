#pragma once

#include "ast.hpp"
#include "environment.hpp"

class Interpreter : public Visitor {
public:
    Interpreter();

    void interpret(Node* node);

    void visit_print_stmt(PrintStmt* stmt) override;
    void visit_expr_stmt(ExprStmt* stmt) override;
    void visit_var_stmt(VarStmt* stmt) override;

    void visit_unary_expr(UnaryExpr* node) override;
    void visit_binary_expr(BinaryExpr* node) override;
    void visit_grouping_expr(GroupingExpr* node) override;
    void visit_literal_expr(LiteralExpr* node) override;
    void visit_comma_expr(CommaExpr* node) override;
    void visit_logical_expr(LogicalExpr* node) override;
    void visit_conditional_expr(ConditionalExpr* node) override;
    void visit_variable_expr(VariableExpr* node) override;
    void visit_assignment_expr(AssignmentExpr* node) override;

private:
    std::vector<ScriptValue> value_stack;
    ScriptEnvironment environment;

    void print_stack();

    bool is_true(ScriptValue& value);
    bool is_equal(ScriptValue& a, ScriptValue& b);

    void assert_value_type(Token op, ScriptValueType type, ScriptValue& value);
    void assert_values_type(Token op, ScriptValueType type, ScriptValue& a, ScriptValue& b);
};
