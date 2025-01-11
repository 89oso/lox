#include "script/ast_dumper.hpp"

// #include <format>
#include <fmt/core.h>

#include <iostream>

AstDumper::AstDumper()
    : indent{ 0 } {
    buffer.reserve(1000);
}

void AstDumper::dump(Node* node) {
    u32 indent_save = indent;

    write("{", true, true);

    node->accept(this);

    indent = indent_save;

    write("}", true, true);

    std::cout << buffer;
}

void AstDumper::visit_print_stmt(PrintStmt* stmt) {
    indent++;

    write_str_field("type", "PrintStmt");
    write_node("expression", stmt->expr.get());
}

void AstDumper::visit_expr_stmt(ExprStmt* stmt) {
    indent++;

    write_str_field("type", "ExprStmt");
    write_node("expression", stmt->expr.get());
}

void AstDumper::visit_var_stmt(VarStmt* stmt) {
    indent++;

    write_str_field("type", "VarStmt");
    write_str_field("name", stmt->name.value);
    write_node("initializer", stmt->initializer.get());
}

void AstDumper::visit_block_stmt(BlockStmt* stmt) {
    indent++;

    write_str_field("type", "BlockStmt");
    write_node_array("statements", stmt->statements);
}

void AstDumper::visit_if_stmt(IfStmt* stmt) {
    indent++;

    write_str_field("type", "IfStmt");
    write_node("condition", stmt->condition.get());
    write_node("then_branch", stmt->then_branch.get());
    write_node("else_branch", stmt->else_branch.get());
}

void AstDumper::visit_while_stmt(WhileStmt* stmt) {
    indent++;

    write_str_field("type", "WhileStmt");
    write_node("condition", stmt->condition.get());
    write_node("body", stmt->body.get());
}

void AstDumper::visit_break_stmt(BreakStmt* stmt) {
    indent++;

    write_str_field("type", "BreakStmt");
}

void AstDumper::visit_function_stmt(FunctionStmt* stmt) {
    indent++;

    write_str_field("type", "FunctionStmt");
    if (stmt->name.type == TokenType::TT_INVALID) {
        write_str_field("name", "<anonymous function>");
    } else {
        write_str_field("name", stmt->name.value);
    }
    write_token_array("params", stmt->params);
    write_node_array("body", stmt->body);
}

void AstDumper::visit_return_stmt(ReturnStmt* stmt) {
    indent++;

    write_str_field("type", "ReturnStmt");
    write_node("expr", stmt->expr.get());
}

void AstDumper::visit_class_stmt(ClassStmt* stmt) {
    indent++;

    write_str_field("type", "ClassStmt");
    write_str_field("name", stmt->name.value);
    write_node_array("functions", stmt->functions);
}

void AstDumper::visit_unary_expr(UnaryExpr* node) {
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

void AstDumper::visit_binary_expr(BinaryExpr* node) {
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
    case TokenType::TT_GREATER:
        write(">");
        break;
    case TokenType::TT_GREATER_EQUAL:
        write(">=");
        break;
    case TokenType::TT_LESS:
        write("<");
        break;
    case TokenType::TT_LESS_EQUAL:
        write("<=");
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

void AstDumper::visit_grouping_expr(GroupingExpr* node) {
    indent++;

    write_str_field("type", "GroupingExpr");
    write_node("expression", node->expr.get());
}

void AstDumper::visit_literal_expr(LiteralExpr* node) {
    indent++;

    write_str_field("type", "LiteralExpr");

    if (node->literal_type == LiteralExpr::LiteralType::Nil) {
        write_str_field("variable_type", "nil");
    } else if (node->literal_type == LiteralExpr::LiteralType::Boolean) {
        write_str_field("variable_type", "boolean");
        write_str_field("variable_value", std::get<bool>(node->value) ? "true" : "false");
    } else if (node->literal_type == LiteralExpr::LiteralType::Number) {
        write_str_field("variable_type", "number");
        write_double_field("variable_value", std::get<f64>(node->value));
    } else if (node->literal_type == LiteralExpr::LiteralType::String) {
        write_str_field("variable_type", "string");
        write_str_field("variable_value", std::get<std::string>(node->value));
    }
}

void AstDumper::visit_logical_expr(LogicalExpr* node) {
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

void AstDumper::visit_conditional_expr(ConditionalExpr* node) {
    indent++;

    write_str_field("type", "ConditionalExpr");

    write_node("expr", node->expr.get());
    write_node("left", node->left.get());
    write_node("right", node->right.get());
}

void AstDumper::visit_variable_expr(VariableExpr* node) {
    indent++;

    write_str_field("type", "VariableExpr");
    write_str_field("name", node->name.value);
}

void AstDumper::visit_assignment_expr(AssignmentExpr* node) {
    indent++;

    write_str_field("type", "AssignmentExpr");
    write_str_field("name", node->name.value);
    write_node("value", node->value.get());
}

void AstDumper::visit_call_expr(CallExpr* node) {
    indent++;

    write_str_field("type", "CallExpr");
    write_node("callee", node->callee.get());
    write_node_array("arguments", node->arguments);
}

void AstDumper::write(const std::string& str, bool indentation, bool newline) {
    if (indentation) {
        for (u32 i = 0; i < indent; i++)
            buffer += "    ";
    }

    buffer += str;

    if (newline)
        buffer += "\n";
}

void AstDumper::write_str_field(const std::string& field, const std::string& value) {
    write("\"" + field + "\"", true);
    write(": ");
    write("\"" + value + "\"", false);
    write(",", false, true);
}

void AstDumper::write_int_field(const std::string& field, const i32 value) {
    write("\"" + field + "\"", true);
    write(": ");
    write("\"" + std::to_string(value) + "\"", false);
    write(",", false, true);
}

void AstDumper::write_double_field(const std::string& field, const f64 value) {
    write("\"" + field + "\"", true);
    write(": ");
    write("\"" + fmt::format("{}", value) + "\"", false);
    write(",", false, true);
}

void AstDumper::write_node(const std::string& name, Node* node) {
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

void AstDumper::write_node_array(const std::string& name, std::vector<Node::ptr>& nodes) {
    write("\"" + name + "\"", true);

    if (nodes.empty()) {
        write(": null,", false, true);
    } else {
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
}

void AstDumper::write_token_array(const std::string& name, std::vector<Token>& tokens) {
    write("\"" + name + "\"", true);

    if (tokens.empty()) {
        write(": null,", false, true);
    } else {
        write(": {", false, true);
        u32 indent_save = indent;
        indent++;
        u32 index = 0;
        for (auto& token : tokens) {
            write_str_field(std::to_string(index), token.value);
            index++;
        }
        indent = indent_save;
        write("},", true, true);
    }
}
