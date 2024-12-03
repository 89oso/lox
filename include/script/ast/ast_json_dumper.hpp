#pragma once

#include "expr.hpp"

struct AstJsonDumper : public Visitor {
    u32 indent;
    std::string buffer;

    AstJsonDumper();

    void write(const std::string& str, bool indentation = false, bool newline = false);
    void write_str_field(const std::string& field, const std::string& value);
    void write_int_field(const std::string& field, const i32 value);
    void write_node(const std::string& name, Node* node);
    void write_node_array(const std::string& name, std::vector<Node::ptr>& nodes);

    std::string dump(Node* node);

    void visit(UnaryExpr* node) override;
    void visit(BinaryExpr* node) override;
    void visit(GroupingExpr* node) override;
    void visit(LiteralExpr* node) override;
    void visit(CommaExpr* node) override;
};
