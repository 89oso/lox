#include "script/ast/ast_json_dumper.hpp"

AstJsonDumper::AstJsonDumper()
    : indent{ 0 } {
    buffer.resize(1000);
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

void AstJsonDumper::write_node(const std::string& name, Node* node) {
    write("\"" + name + "\"", true);
    write(": {", false, true);

    u32 indent_save = indent;

    node->accept(this);

    indent = indent_save;

    write("},", true, true);
}

std::string AstJsonDumper::dump(Node* node) {
    u32 indent_save = indent;

    write("{", true, true);

    node->accept(this);

    indent = indent_save;

    write("}", true, true);

    return buffer;
}

void AstJsonDumper::visit(UnaryExpr* node) {
    indent++;

    write_str_field("type", "UnaryExpr");

    write("\"operator\": \"", true);
    switch (node->op) {
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

void AstJsonDumper::visit(BinaryExpr* node) {
    indent++;

    write_str_field("type", "BinaryExpr");

    write("\"operator\": \"", true);
    switch (node->op) {
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

void AstJsonDumper::visit(GroupingExpr* node) {
    indent++;

    write_str_field("type", "GroupingExpr");
    write_node("expression", node->expr.get());
}

void AstJsonDumper::visit(LiteralExpr* node) {
    indent++;

    write_str_field("type", "LiteralExpr");
    write_str_field("value", node->value);
}
