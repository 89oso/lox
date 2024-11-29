#include "writer.hpp"
#include <vector>

struct VariableDef {
    std::string type;
    std::string name;
};

struct ClassDef {
    std::string name;
    std::string base;
    std::vector<VariableDef> vars;
};

static std::vector<ClassDef> expr_types;

static void create_expr_types(const std::string& base) {
    auto& types = expr_types;

    types.push_back({ "Unary", base });
    types.back().vars.push_back({ "TokenType", "op" });
    types.back().vars.push_back({ "Node::ptr", "expr" });

    types.push_back({ "Binary", base });
    types.back().vars.push_back({ "TokenType", "op" });
    types.back().vars.push_back({ "Node::ptr", "left" });
    types.back().vars.push_back({ "Node::ptr", "right" });

    types.push_back({ "Grouping", base });
    types.back().vars.push_back({ "Node::ptr", "expr" });

    types.push_back({ "Literal", base });
    types.back().vars.push_back({ "std::string", "value" });
}

static void create_types() {
    create_expr_types("Expr");
}

static void generate_node_file() {
    Writer writer;

    writer.write_line(0, "#pragma once");
    writer.write_line();
    writer.write_line(0, "#include \"visitor.hpp\"");
    writer.write_line();

    writer.write_line(0, "struct Node {");
    writer.write_line(1, "using ptr = std::unique_ptr<Node>;");
    writer.write_line();
    writer.write_line(1, "virtual ~Node() = default;");
    writer.write_line(1, "virtual void accept(Visitor* visitor) = 0;");
    writer.write_line(0, "};");

    writer.flush_to_file("../src/script/ast/node.hpp");
}

static void generate_visitor_file() {
    Writer writer;

    writer.write_line(0, "#pragma once");
    writer.write_line();

    for (auto& type : expr_types) {
        writer.write_line(0, "struct " + type.name + type.base + ";");
    }

    writer.write_line();

    writer.write_line(0, "struct Visitor {");
    writer.write_line(1, "virtual ~Visitor() = default;");
    writer.write_line();

    for (auto& type : expr_types) {
        writer.write_line(1, "virtual void visit(" + type.name + type.base + "* node) = 0;");
    }

    writer.write_line(0, "};");

    writer.flush_to_file("../src/script/ast/visitor.hpp");
}

static void write_expr_types(Writer& writer, const std::string& base) {
    writer.write_line(0, "struct " + base + " : public Node {");
    writer.write_line(1, "virtual ~" + base + "() = default;");
    writer.write_line(1, "virtual void accept(Visitor* visitor) = 0;");
    writer.write_line(0, "};");

    writer.write_line();

    auto& types = expr_types;
    for (auto& type : types) {
        writer.write_line(0, "struct " + type.name + type.base + " : " + type.base + " {");

        for (auto& var : type.vars) {
            writer.write_line(1, var.type + " " + var.name + ";");
        }

        writer.write_line();

        // write the constructor
        {
            writer.write_line(1, type.name + type.base + "() = default;");
            writer.write_line();

            writer.write(1, type.name + type.base + "(");

            for (auto& var : type.vars) {
                writer.write(0, var.type + " " + var.name);
                if (&var != &type.vars.back()) {
                    writer.write(0, ", ");
                }
            }

            writer.write(0, ") : ");

            for (auto& var : type.vars) {
                writer.write(0, var.name + "(" + var.name + ")");
                if (&var != &type.vars.back()) {
                    writer.write(0, ", ");
                }
            }

            // braces for constructor
            writer.write_line(0, " {");
            writer.write_line(1, "}");

            writer.write_line();
        }

        writer.write_line(1, "void accept(Visitor* visitor) override {");
        writer.write_line(2, "visitor->visit(this);");
        writer.write_line(1, "}");

        writer.write_line(0, "};");

        if (&type != &types.back())
            writer.write_line();
    }
}

static void generate_expr_file() {
    Writer writer;

    writer.write_line(0, "#pragma once");
    writer.write_line();
    writer.write_line(0, "#include \"../token.hpp\"");
    writer.write_line(0, "#include \"node.hpp\"");
    writer.write_line();

    write_expr_types(writer, "Expr");

    writer.flush_to_file("../src/script/ast/expr.hpp");
}

int main() {
    create_types();

    generate_node_file();
    generate_visitor_file();

    generate_expr_file();

    return 0;
}
