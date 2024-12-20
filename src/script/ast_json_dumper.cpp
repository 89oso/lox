#include "script/ast_json_dumper.hpp"

#include <format>
#include <iostream>

AstJsonDumper::AstJsonDumper()
    : indent{ 0 } {
    buffer.resize(1000);
}

void AstJsonDumper::dump(Node* node) {
    u32 indent_save = indent;

    write("{", true, true);

    node->accept(this);

    indent = indent_save;

    write("}", true, true);

    std::cout << buffer;
}

void AstJsonDumper::visit_print_stmt(PrintStmt* stmt) {
    indent++;

    write_str_field("type", "PrintStmt");
    write_node("expression", stmt->expr.get());
}

void AstJsonDumper::visit_expr_stmt(ExprStmt* stmt) {
    indent++;

    write_str_field("type", "ExprStmt");
    write_node("expression", stmt->expr.get());
}

void AstJsonDumper::visit_var_stmt(VarStmt* stmt) {
    indent++;

    write_str_field("type", "VarStmt");
    write_str_field("name", stmt->name.value);
    write_node("initializer", stmt->initializer.get());
}

void AstJsonDumper::visit_unary_expr(UnaryExpr* node) {
    indent++;

    write_str_field("type", "UnaryExpr");

    write("\"operator\": \"", true);
    switch (node->op.type) {
    case TokenType::TT_BANG:
        write("!");
        break;
    case TokenType::TT_MINUS:
        write("-");
        break;
    default:
        break;
    }
    write("\",", false, true);

    write_node("expression", node->expr.get());
}

void AstJsonDumper::visit_binary_expr(BinaryExpr* node) {
    indent++;

    write_str_field("type", "BinaryExpr");

    write("\"operator\": \"", true);
    switch (node->op.type) {
    case TokenType::TT_PLUS:
        write("+");
        break;
    case TokenType::TT_MINUS:
        write("-");
        break;
    case TokenType::TT_STAR:
        write("*");
        break;
    case TokenType::TT_SLASH:
        write("/");
        break;
    case TokenType::TT_EQUAL_EQUAL:
        write("==");
        break;
    case TokenType::TT_BANG_EQUAL:
        write("!=");
        break;
    default:
        break;
    }
    write("\",", false, true);

    write_node("left", node->left.get());
    write_node("right", node->right.get());
}

void AstJsonDumper::visit_grouping_expr(GroupingExpr* node) {
    indent++;

    write_str_field("type", "GroupingExpr");
    write_node("expression", node->expr.get());
}

void AstJsonDumper::visit_literal_expr(LiteralExpr* node) {
    indent++;

    write_str_field("type", "LiteralExpr");

    if (node->literal_type == LiteralExpr::LiteralType::Nil) {
        write_str_field("variable_type", "nil");
    } else if (node->literal_type == LiteralExpr::LiteralType::Boolean) {
        write_str_field("variable_type", "boolean");
        write_str_field("variable_value", node->value.boolean ? "true" : "false");
    } else if (node->literal_type == LiteralExpr::LiteralType::Number) {
        write_str_field("variable_type", "number");
        write_double_field("variable_value", node->value.number);
    } else if (node->literal_type == LiteralExpr::LiteralType::String) {
        write_str_field("variable_type", "string");
        write_str_field("variable_value", node->value.string);
    }
}

void AstJsonDumper::visit_comma_expr(CommaExpr* node) {
    indent++;

    write_str_field("type", "CommaExpr");
    write_node_array("expressions", node->expressions);
}

void AstJsonDumper::visit_logical_expr(LogicalExpr* node) {
    indent++;

    write_str_field("type", "LogicalExpr");

    write("\"operator\": \"", true);
    switch (node->op.type) {
    case TokenType::TT_AND:
        write("and");
        break;
    case TokenType::TT_OR:
        write("or");
        break;
    default:
        break;
    }
    write("\",", false, true);

    write_node("left", node->left.get());
    write_node("right", node->right.get());
}

void AstJsonDumper::visit_conditional_expr(ConditionalExpr* node) {
    indent++;

    write_str_field("type", "ConditionalExpr");

    write_node("expr", node->expr.get());
    write_node("left", node->left.get());
    write_node("right", node->right.get());
}

void AstJsonDumper::visit_variable_expr(VariableExpr* node) {
    indent++;

    write_str_field("type", "VariableExpr");
    write_str_field("name", node->name.value);
}

void AstJsonDumper::visit_assignment_expr(AssignmentExpr* node) {
    indent++;

    write_str_field("type", "AssignmentExpr");
    write_str_field("name", node->name.value);
    write_node("value", node->value.get());
}

void AstJsonDumper::write(const std::string& str, bool indentation, bool newline) {
    if (indentation) {
        for (u32 i = 0; i < indent; i++)
            buffer += "    ";
    }

    buffer += str;

    if (newline)
        buffer += "\n";
}

void AstJsonDumper::write_str_field(const std::string& field, const std::string& value) {
    write("\"" + field + "\"", true);
    write(": ");
    write("\"" + value + "\"", false);
    write(",", false, true);
}

void AstJsonDumper::write_int_field(const std::string& field, const i32 value) {
    write("\"" + field + "\"", true);
    write(": ");
    write("\"" + std::to_string(value) + "\"", false);
    write(",", false, true);
}

void AstJsonDumper::write_double_field(const std::string& field, const f64 value) {
    write("\"" + field + "\"", true);
    write(": ");
    write("\"" + std::format("{}", value) + "\"", false);
    write(",", false, true);
}

void AstJsonDumper::write_node(const std::string& name, Node* node) {
    write("\"" + name + "\"", true);

    if (node) {
        write(": {", false, true);

        u32 indent_save = indent;
        node->accept(this);
        indent = indent_save;

        write("},", true, true);
    } else {
        write(": null,", false, true);
    }
}

void AstJsonDumper::write_node_array(const std::string& name, std::vector<Node::ptr>& nodes) {
    write("\"" + name + "\"", true);
    write(": {", false, true);

    u32 indent_save = indent;

    indent++;

    u32 index = 0;
    for (auto& node : nodes) {
        write_node("node" + std::to_string(index), node.get());
        index++;
    }

    indent = indent_save;

    write("},", true, true);
}
