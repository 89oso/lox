#include "script/interpreter.hpp"

#include <iostream>

Interpreter::Interpreter() {
    variable_stack.reserve(100);
}

void Interpreter::print_stack() {
    std::cout << "---- VARIABLE STACK ----\n";

    std::cout << "Variables (" << variable_stack.size() << "):\n";

    for (size_t i = 0; i < variable_stack.size(); i++) {
        std::cout << "    " << i << ": ";

        auto& variable = variable_stack[i];
        if (variable.type == ScriptVariableType::Nil) {
            std::cout << "nil";
        } else if (variable.type == ScriptVariableType::Boolean) {
            std::cout << "boolean -> " << variable.value.boolean;
        } else if (variable.type == ScriptVariableType::Number) {
            std::cout << "number -> " << variable.value.number;
        } else if (variable.type == ScriptVariableType::String) {
            std::cout << "string -> " << variable.value.string;
        }

        std::cout << "\n";
    }

    std::cout << "------------------------\n";
}

void Interpreter::visit(UnaryExpr* node) {
}

void Interpreter::visit(BinaryExpr* node) {
}

void Interpreter::visit(GroupingExpr* node) {
}

// TODO: strings currently come from the token created by the lexer (std::string)
//     : maybe an arena for the whole AST would be a good idea?
void Interpreter::visit(LiteralExpr* node) {
    ScriptVariable variable;

    if (node->type == LiteralExpr::Type::Nil) {
        variable.type = ScriptVariableType::Nil;
    } else if (node->type == LiteralExpr::Type::Boolean) {
        variable.type = ScriptVariableType::Boolean;
        variable.value.boolean = node->boolean;
    } else if (node->type == LiteralExpr::Type::Number) {
        variable.type = ScriptVariableType::Number;
        variable.value.number = node->number;
    } else if (node->type == LiteralExpr::Type::String) {
        variable.type = ScriptVariableType::String;
        variable.value.string = node->string;
    }

    variable_stack.push_back(variable);
}

void Interpreter::visit(CommaExpr* node) {
    // TODO
}

void Interpreter::visit(LogicalExpr* node) {
    // TODO
}

void Interpreter::visit(ConditionalExpr* node) {
    // TODO
}
