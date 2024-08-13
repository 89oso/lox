#include <iostream>
#include <string>
#include <vector>

struct VariableDef {
    std::string type;
    std::string name;

    // VariableDef(const std::string& _type, const std::string& _name) {
    //     type = _type;
    //     name = _name;
    // }
};

struct ClassDef {
    std::string name;
    std::string base;
    std::vector<VariableDef> vars;
};

std::vector<ClassDef> expr_definitions;

static void write_line(std::string& str, std::string line = "") {
    if (!line.empty())
        str += line;
    str += "\n";
}

static void generate_expr(std::string& output, const std::string& base_class) {
    write_line(output, "    class " + base_class + "{");
    write_line(output, "    };");

    auto& definitions = expr_definitions;

    definitions.push_back({ "Binary", base_class });
    definitions.back().vars.push_back({ "Expr", "left" });
    definitions.back().vars.push_back({ "Expr", "right" });
    definitions.back().vars.push_back({ "Token", "operator" });

    definitions.push_back({ "Grouping", base_class });
    definitions.back().vars.push_back({ "Expr", "expression" });

    definitions.push_back({ "Literal", base_class });
    definitions.back().vars.push_back({ "std::string", "value" });

    write_line(output);
}

static void define_ast(std::string& output, const std::string& base_class) {
    write_line(output, "#pragma once");
    write_line(output);

    write_line(output, "namespace lox::ast {");

    generate_expr(output, "Expr");

    write_line(output, "}");
}

int main() {
    std::string output;
    define_ast(output, "Expr");

    std::cout << output;

    return 0;
}
