#pragma once

#include "ast.hpp"

class AstJsonDumper : public Visitor {
public:
    AstJsonDumper();

    void dump(Node* node);

    void visit_print_stmt(PrintStmt* stmt) override;
    void visit_expr_stmt(ExprStmt* stmt) override;
    void visit_var_stmt(VarStmt* stmt) override;
    void visit_block_stmt(BlockStmt* stmt) override;
    void visit_if_stmt(IfStmt* stmt) override;
    void visit_while_stmt(WhileStmt* stmt) override;
    void visit_break_stmt(BreakStmt* stmt) override;
    void visit_function_stmt(FunctionStmt* stmt) override;

    void visit_unary_expr(UnaryExpr* node) override;
    void visit_binary_expr(BinaryExpr* node) override;
    void visit_grouping_expr(GroupingExpr* node) override;
    void visit_literal_expr(LiteralExpr* node) override;
    void visit_logical_expr(LogicalExpr* node) override;
    void visit_conditional_expr(ConditionalExpr* node) override;
    void visit_variable_expr(VariableExpr* node) override;
    void visit_assignment_expr(AssignmentExpr* node) override;
    void visit_call_expr(CallExpr* node) override;

private:
    u32 indent;
    std::string buffer;

    void write(const std::string& str, bool indentation = false, bool newline = false);
    void write_str_field(const std::string& field, const std::string& value);
    void write_int_field(const std::string& field, const i32 value);
    void write_double_field(const std::string& field, const f64 value);
    void write_node(const std::string& name, Node* node);
    void write_node_array(const std::string& name, std::vector<Node::ptr>& nodes);
    void write_token_array(const std::string& name, std::vector<Token>& tokens);
};
