#pragma once

#include "ast.hpp"

struct AstJsonDumper : public Visitor {
    u32 indent;
    std::string buffer;

    AstJsonDumper();

    void write(const std::string& str, bool indentation = false, bool newline = false);
    void write_str_field(const std::string& field, const std::string& value);
    void write_int_field(const std::string& field, const i32 value);
    void write_double_field(const std::string& field, const f64 value);
    void write_node(const std::string& name, Node* node);
    void write_node_array(const std::string& name, std::vector<Node::ptr>& nodes);

    std::string dump(Node* node);

    void visit_unary_expr(UnaryExpr* node) override;
    void visit_binary_expr(BinaryExpr* node) override;
    void visit_grouping_expr(GroupingExpr* node) override;
    void visit_literal_expr(LiteralExpr* node) override;
    void visit_comma_expr(CommaExpr* node) override;
    void visit_logical_expr(LogicalExpr* node) override;
    void visit_conditional_expr(ConditionalExpr* node) override;
};
