#include "script/interpreter.hpp"
#include "common/exception.hpp"

#include <iostream>

Interpreter::Interpreter() {
    variable_stack.reserve(100);
}

void Interpreter::interpret(Node* node) {
    try {
        node->accept(this);
        print_stack();
    } catch (const std::exception& e) {
        std::cerr << "[runtime error]: " << e.what() << "\n";
    }
}

void Interpreter::visit(UnaryExpr* node) {
    // evaluate the sub-expression first
    node->expr->accept(this);

    auto& variable = variable_stack.back();
    if (node->op == TokenType::TT_MINUS) {
        assert_variable_type(node->op, ScriptVariableType::Number, variable);
        variable.value.number = -variable.value.number;
    } else if (node->op == TokenType::TT_BANG) {
        variable.type = ScriptVariableType::Boolean;
        variable.value.boolean = !is_true(variable);
    }
}

void Interpreter::visit(BinaryExpr* node) {
    node->left->accept(this);
    auto& left = variable_stack.back();

    node->right->accept(this);
    auto& right = variable_stack.back();

    ScriptVariable variable;

    switch (node->op) {
    case TokenType::TT_PLUS: {
        if (left.type == ScriptVariableType::Number) {
            if (right.type != ScriptVariableType::Number) {
                throw RuntimeError(node->op, "rhs of number addition must be a number");
                return;
            }

            variable.type = ScriptVariableType::Number;
            variable.value.number = left.value.number + right.value.number;
        } else if (left.type == ScriptVariableType::String) {
            if (right.type != ScriptVariableType::Number && right.type != ScriptVariableType::String) {
                throw RuntimeError(node->op, "rhs of string concatenation must be a number or a string");
                return;
            }

            std::string rhs_string =
                right.type == ScriptVariableType::Number ? std::to_string(right.value.number) : right.value.string;

            variable.type = ScriptVariableType::String;
            variable.value.string = left.value.string + rhs_string;
        } else {
            throw RuntimeError(node->op, "only numbers and strings are allowed for binary expressions");
            return;
        }
    } break;
    case TokenType::TT_MINUS:
        assert_variables_type(node->op, ScriptVariableType::Number, left, right);
        variable.type = ScriptVariableType::Number;
        variable.value.number = left.value.number - right.value.number;
        break;
    case TokenType::TT_STAR:
        assert_variables_type(node->op, ScriptVariableType::Number, left, right);
        variable.type = ScriptVariableType::Number;
        variable.value.number = left.value.number * right.value.number;
        break;
    case TokenType::TT_SLASH:
        assert_variables_type(node->op, ScriptVariableType::Number, left, right);
        variable.type = ScriptVariableType::Number;
        variable.value.number = left.value.number / right.value.number;
        break;
    case TokenType::TT_GREATER:
        assert_variables_type(node->op, ScriptVariableType::Number, left, right);
        variable.type = ScriptVariableType::Boolean;
        variable.value.boolean = left.value.number > right.value.number;
        break;
    case TokenType::TT_GREATER_EQUAL:
        assert_variables_type(node->op, ScriptVariableType::Number, left, right);
        variable.type = ScriptVariableType::Boolean;
        variable.value.boolean = left.value.number >= right.value.number;
        break;
    case TokenType::TT_LESS:
        assert_variables_type(node->op, ScriptVariableType::Number, left, right);
        variable.type = ScriptVariableType::Boolean;
        variable.value.boolean = left.value.number < right.value.number;
        break;
    case TokenType::TT_LESS_EQUAL:
        assert_variables_type(node->op, ScriptVariableType::Number, left, right);
        variable.type = ScriptVariableType::Boolean;
        variable.value.boolean = left.value.number <= right.value.number;
        break;
    case TokenType::TT_BANG_EQUAL:
        variable.type = ScriptVariableType::Boolean;
        variable.value.boolean = !is_equal(left, right);
        break;
    case TokenType::TT_EQUAL_EQUAL:
        variable.type = ScriptVariableType::Boolean;
        variable.value.boolean = is_equal(left, right);
        break;
    default:
        throw RuntimeError(node->op, "unknown binary expression operator");
        break;
    }

    // remove last element from var stack
    variable_stack.pop_back();

    // overwrite stack top with our new variable
    left = variable;
}

void Interpreter::visit(GroupingExpr* node) {
    // maybe assert the stack size stays the same?
    node->expr->accept(this);
}

// TODO: strings currently come from the token created by the lexer (std::string)
//     : maybe an arena for the whole AST would be a good idea?
void Interpreter::visit(LiteralExpr* node) {
    ScriptVariable variable;

    if (node->type == LiteralExpr::Type::Nil) {
        variable.type = ScriptVariableType::Nil;
    } else if (node->type == LiteralExpr::Type::Boolean) {
        variable.type = ScriptVariableType::Boolean;
        variable.value.boolean = node->value.boolean;
    } else if (node->type == LiteralExpr::Type::Number) {
        variable.type = ScriptVariableType::Number;
        variable.value.number = node->value.number;
    } else if (node->type == LiteralExpr::Type::String) {
        variable.type = ScriptVariableType::String;
        variable.value.string = node->value.string;
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

bool Interpreter::is_true(ScriptVariable& variable) {
    if (variable.type == ScriptVariableType::Nil)
        return false;
    else if (variable.type == ScriptVariableType::Boolean)
        return variable.value.boolean;
    return true;
}

bool Interpreter::is_equal(ScriptVariable& a, ScriptVariable& b) {
    if (a.type != b.type)
        return false;
    else if (a.type == ScriptVariableType::Nil && b.type == ScriptVariableType::Nil)
        return true;
    else if (a.type == ScriptVariableType::Nil)
        return false;
    else if (a.type == ScriptVariableType::Number)
        return a.value.number == b.value.number;
    else if (a.type == ScriptVariableType::String)
        return a.value.string == b.value.string;
    return false;
}

void Interpreter::assert_variable_type(Token op, ScriptVariableType type, ScriptVariable& variable) {
    if (variable.type == type)
        return;
    throw RuntimeError(op, "variable type mismatch");
}

void Interpreter::assert_variables_type(Token op, ScriptVariableType type, ScriptVariable& a, ScriptVariable& b) {
    if (a.type == type && b.type == type)
        return;
    throw RuntimeError(op, "variable type mismatch");
}
