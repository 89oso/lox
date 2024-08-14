#include <iostream>
#include <string>
#include <vector>

class Writer {
public:
    void write(int indent, std::string str) {
        for (auto i = 0; i < indent; i++)
            _buf += "    ";
        _buf += str;
    }

    void write_line(int indent = 0, std::string str = "") {
        for (auto i = 0; i < indent; i++)
            _buf += "    ";
        _buf += str;
        _buf += "\n";
    }

    const std::string& get_buffer() const {
        return _buf;
    }

private:
    std::string _buf;
};

struct VariableDef {
    std::string type;
    std::string name;
};

struct ClassDef {
    std::string name;
    std::string base;
    std::vector<VariableDef> vars;
};

static Writer writer;

static std::vector<ClassDef> expr_types;

static void create_expr_types(const std::string& base) {
    auto& types = expr_types;

    types.push_back({ "Unary", base });
    types.back().vars.push_back({ "Token", "op" });
    types.back().vars.push_back({ "Expr", "right" });

    types.push_back({ "Binary", base });
    types.back().vars.push_back({ "Expr", "left" });
    types.back().vars.push_back({ "Expr", "right" });
    types.back().vars.push_back({ "Token", "op" });

    types.push_back({ "Grouping", base });
    types.back().vars.push_back({ "Expr", "expression" });

    types.push_back({ "Literal", base });
    types.back().vars.push_back({ "std::string", "value" });
}

static void create_types() {
    create_expr_types("Expr");
}

static void write_expr_types(const std::string& base) {
    writer.write_line(1, "class " + base + " {");
    writer.write_line(1, "};");

    writer.write_line();

    auto& types = expr_types;
    for (auto& type : types) {
        writer.write_line(1, "class " + type.base + type.name + " : public " + type.base + " {");
        writer.write_line(1, "public:");

        // write the constructor
        {
            writer.write(2, type.base + type.name + "(");
            for (auto& var : type.vars) {
                writer.write(0, var.type + " " + var.name);
                if (&var != &type.vars.back()) {
                    writer.write(0, ", ");
                }
            }
            writer.write(0, ")");
        }

        // write initializer list for constructor
        {
            // TODO: store implementation in .cpp file instead of everything in header
            writer.write(0, " : ");

            for (auto& var : type.vars) {
                writer.write(0, var.name + "(" + var.name + ")");
                if (&var != &type.vars.back()) {
                    writer.write(0, ", ");
                }
            }

            // braces for constructor
            writer.write_line(0, " {");
            writer.write_line(2, "}");
        }

        writer.write_line();

        // write variables declarations
        {
            for (auto& var : type.vars) {
                writer.write_line(2, var.type + " " + var.name + ";");
            }
        }

        writer.write_line(1, "};");

        if (&type != &types.back())
            writer.write_line();
    }
}

static void write_types() {
    write_expr_types("Expr");
}

int main() {
    create_types();

    writer.write_line(0, "#pragma once");
    writer.write_line();
    writer.write_line(0, "#include \"../token.hpp\"");
    writer.write_line();

    writer.write_line(0, "namespace lox::ast {");

    write_types();

    writer.write_line(0, "}");

    std::cout << writer.get_buffer();

    return 0;
}
