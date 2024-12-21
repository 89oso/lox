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
    void visit_block_stmt(BlockStmt* stmt) override;

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
    ScriptEnvironment _global_env;
    ScriptEnvironment* _current_env;

    std::vector<ScriptValue> _value_stack;

    void print_stack();
    void execute_block(std::vector<Node::ptr>& statements, std::unique_ptr<ScriptEnvironment> environment);

    bool is_true(ScriptValue& value);
    bool is_equal(ScriptValue& a, ScriptValue& b);

    void assert_value_type(Token op, ScriptValueType type, ScriptValue& value);
    void assert_values_type(Token op, ScriptValueType type, ScriptValue& a, ScriptValue& b);
};
