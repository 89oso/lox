#include "script/interpreter.hpp"
#include "common/exception.hpp"
#include "common/finally.hpp"

#include <iostream>
#include <format>

Interpreter::Interpreter() {
    _current_env = &_global_env;
    _value_stack.reserve(100);
}

void Interpreter::interpret(Node* node) {
    try {
        node->accept(this);
        // print_stack();
    } catch (const RuntimeError& e) {
        std::cerr << "[runtime error]: " << e.what() << "\n";
    }
}

void Interpreter::visit_print_stmt(PrintStmt* stmt) {
    // evaluate the expression then print the variable on the top of the stack
    stmt->expr->accept(this);

    auto& variable = _value_stack.back();

    std::cout << "[runtime]: ";

    if (variable.type == ScriptValueType::Nil) {
        std::cout << "nil";
    } else if (variable.type == ScriptValueType::Boolean) {
        std::cout << (std::get<bool>(variable.value) ? "true" : "false");
    } else if (variable.type == ScriptValueType::Number) {
        std::cout << std::get<f64>(variable.value);
    } else if (variable.type == ScriptValueType::String) {
        std::cout << std::get<std::string>(variable.value);
    }

    std::cout << "\n";

    _value_stack.pop_back();
}

void Interpreter::visit_expr_stmt(ExprStmt* stmt) {
    // evaluate the expr
    stmt->expr->accept(this);

    // values created by the expr stmt do not need to stay in the value stack
    _value_stack.pop_back();
}

void Interpreter::visit_var_stmt(VarStmt* stmt) {
    // value is nil by default (ScriptValue constructor)
    ScriptValue value;

    // if an initializer expr is present then evaluate it
    if (stmt->initializer) {
        stmt->initializer->accept(this);
        value = _value_stack.back();
    }

    _current_env->define_variable(stmt->name.value, value);
}

void Interpreter::visit_block_stmt(BlockStmt* stmt) {
    auto environment = std::make_unique<ScriptEnvironment>(_current_env);
    execute_block(stmt->statements, std::move(environment));
}

void Interpreter::visit_if_stmt(IfStmt* stmt) {
    stmt->condition->accept(this);

    ScriptValue& value = _value_stack.back();

    if (is_true(value))
        stmt->then_branch->accept(this);
    else if (stmt->else_branch)
        stmt->else_branch->accept(this);
}

void Interpreter::visit_unary_expr(UnaryExpr* node) {
    // evaluate the sub-expression first
    node->expr->accept(this);

    auto& variable = _value_stack.back();
    if (node->op.type == TokenType::TT_MINUS) {
        assert_value_type(node->op, ScriptValueType::Number, variable);
        variable.value = -std::get<f64>(variable.value);
    } else if (node->op.type == TokenType::TT_BANG) {
        variable.type = ScriptValueType::Boolean;
        variable.value = !is_true(variable);
    }
}

void Interpreter::visit_binary_expr(BinaryExpr* node) {
    node->left->accept(this);
    auto& left = _value_stack.back();

    node->right->accept(this);
    auto& right = _value_stack.back();

    ScriptValue variable;

    switch (node->op.type) {
    case TokenType::TT_PLUS: {
        if (left.type == ScriptValueType::Number) {
            if (right.type == ScriptValueType::String) {
                variable.type = ScriptValueType::String;
                variable.value = std::format("{}", std::get<f64>(left.value)) + std::get<std::string>(right.value);
                break;
            }
            assert_value_type(node->op, ScriptValueType::Number, right);
            variable.type = ScriptValueType::Number;
            variable.value = std::get<f64>(left.value) + std::get<f64>(right.value);
        } else if (left.type == ScriptValueType::String) {
            if (right.type == ScriptValueType::Number) {
                variable.type = ScriptValueType::String;
                variable.value = std::get<std::string>(left.value) + std::format("{}", std::get<f64>(right.value));
                break;
            }
            assert_value_type(node->op, ScriptValueType::String, right);
            variable.type = ScriptValueType::String;
            variable.value = std::get<std::string>(left.value) + std::get<std::string>(right.value);
        } else {
            throw RuntimeError(node->op, "only numbers and strings are allowed for binary expressions");
            return;
        }
    } break;
    case TokenType::TT_MINUS:
        assert_values_type(node->op, ScriptValueType::Number, left, right);
        variable.type = ScriptValueType::Number;
        variable.value = std::get<f64>(left.value) - std::get<f64>(right.value);
        break;
    case TokenType::TT_STAR:
        assert_values_type(node->op, ScriptValueType::Number, left, right);
        variable.type = ScriptValueType::Number;
        variable.value = std::get<f64>(left.value) * std::get<f64>(right.value);
        break;
    case TokenType::TT_SLASH: {
        assert_values_type(node->op, ScriptValueType::Number, left, right);
        f64 lhs_value = std::get<f64>(left.value);
        f64 rhs_value = std::get<f64>(right.value);
        if (lhs_value == 0.0 || rhs_value == 0.0) {
            throw RuntimeError(node->op, "division by zero is not allowed");
            return;
        }
        variable.type = ScriptValueType::Number;
        variable.value = lhs_value / rhs_value;
    } break;
    case TokenType::TT_GREATER:
        assert_values_type(node->op, ScriptValueType::Number, left, right);
        variable.type = ScriptValueType::Boolean;
        variable.value = std::get<bool>(left.value) > std::get<bool>(right.value);
        break;
    case TokenType::TT_GREATER_EQUAL:
        assert_values_type(node->op, ScriptValueType::Number, left, right);
        variable.type = ScriptValueType::Boolean;
        variable.value = std::get<bool>(left.value) >= std::get<bool>(right.value);
        break;
    case TokenType::TT_LESS:
        assert_values_type(node->op, ScriptValueType::Number, left, right);
        variable.type = ScriptValueType::Boolean;
        variable.value = std::get<bool>(left.value) < std::get<bool>(right.value);
        break;
    case TokenType::TT_LESS_EQUAL:
        assert_values_type(node->op, ScriptValueType::Number, left, right);
        variable.type = ScriptValueType::Boolean;
        variable.value = std::get<bool>(left.value) <= std::get<bool>(right.value);
        break;
    case TokenType::TT_BANG_EQUAL:
        variable.type = ScriptValueType::Boolean;
        variable.value = !is_equal(left, right);
        break;
    case TokenType::TT_EQUAL_EQUAL:
        variable.type = ScriptValueType::Boolean;
        variable.value = is_equal(left, right);
        break;
    default:
        throw RuntimeError(node->op, "unknown binary expression operator");
        break;
    }

    // remove left and right vars from var stack
    _value_stack.pop_back();

    left = variable;
}

void Interpreter::visit_grouping_expr(GroupingExpr* node) {
    // maybe assert the stack size stays the same?
    node->expr->accept(this);
}

// TODO: strings currently come from the token created by the lexer (std::string)
//     : maybe an arena for the whole AST would be a good idea?
void Interpreter::visit_literal_expr(LiteralExpr* node) {
    ScriptValue variable;

    if (node->literal_type == LiteralExpr::LiteralType::Nil) {
        variable.type = ScriptValueType::Nil;
    } else if (node->literal_type == LiteralExpr::LiteralType::Boolean) {
        variable.type = ScriptValueType::Boolean;
        variable.value = node->value.boolean;
    } else if (node->literal_type == LiteralExpr::LiteralType::Number) {
        variable.type = ScriptValueType::Number;
        variable.value = node->value.number;
    } else if (node->literal_type == LiteralExpr::LiteralType::String) {
        variable.type = ScriptValueType::String;
        variable.value = node->value.string;
    }

    _value_stack.push_back(variable);
}

void Interpreter::visit_comma_expr(CommaExpr* node) {
    // TODO
}

void Interpreter::visit_logical_expr(LogicalExpr* node) {
    node->left->accept(this);

    ScriptValue& left_result = _value_stack.back();

    // TODO: modify the top of the stack? prob not needed
    if (node->op.type == TokenType::TT_OR) {
        if (is_true(left_result))
            return;
    } else {
        if (!is_true(left_result))
            return;
    }

    // left side is good so we must evaluate the right side now
    _value_stack.pop_back();
    node->right->accept(this);
}

void Interpreter::visit_conditional_expr(ConditionalExpr* node) {
    // TODO
}

void Interpreter::visit_variable_expr(VariableExpr* node) {
    ScriptValue value = _current_env->find_variable(node->name);
    _value_stack.push_back(value);
}

void Interpreter::visit_assignment_expr(AssignmentExpr* node) {
    node->value->accept(this);

    auto value = _value_stack.back();
    _current_env->assign_variable(node->name, value);

    // TODO: pop value from stack?
}

void Interpreter::print_stack() {
    std::cout << "---- VARIABLE STACK ----\n";

    std::cout << "Variables (" << _value_stack.size() << "):\n";

    for (size_t i = 0; i < _value_stack.size(); i++) {
        std::cout << "    " << i << ": ";

        auto& variable = _value_stack[i];
        if (variable.type == ScriptValueType::Nil) {
            std::cout << "nil";
        } else if (variable.type == ScriptValueType::Boolean) {
            std::string boolean_value = std::get<bool>(variable.value) ? "true" : "false";
            std::cout << "boolean -> " << boolean_value;
        } else if (variable.type == ScriptValueType::Number) {
            std::cout << "number -> " << std::get<f64>(variable.value);
        } else if (variable.type == ScriptValueType::String) {
            std::cout << "string -> " << std::get<std::string>(variable.value);
        }

        std::cout << "\n";
    }

    std::cout << "------------------------\n";
}

void Interpreter::execute_block(std::vector<Node::ptr>& statements, std::unique_ptr<ScriptEnvironment> environment) {
    ScriptEnvironment* previous_env = _current_env;

    // defer this
    auto _ = oso::finally([&] {
        _current_env = previous_env;
    });

    _current_env = environment.get();
    for (auto& stmt : statements) {
        stmt->accept(this);
    }
}

bool Interpreter::is_true(ScriptValue& variable) {
    if (variable.type == ScriptValueType::Nil)
        return false;
    else if (variable.type == ScriptValueType::Boolean)
        return std::get<bool>(variable.value);
    return true;
}

bool Interpreter::is_equal(ScriptValue& a, ScriptValue& b) {
    if (a.type != b.type)
        return false;
    else if (a.type == ScriptValueType::Nil && b.type == ScriptValueType::Nil)
        return true;
    else if (a.type == ScriptValueType::Nil)
        return false;
    else if (a.type == ScriptValueType::Number)
        return a.value == b.value;
    else if (a.type == ScriptValueType::String)
        return a.value == b.value;
    return false;
}

void Interpreter::assert_value_type(Token op, ScriptValueType type, ScriptValue& variable) {
    if (variable.type == type)
        return;
    throw RuntimeError(op, "variable type mismatch");
}

void Interpreter::assert_values_type(Token op, ScriptValueType type, ScriptValue& a, ScriptValue& b) {
    if (a.type == type && b.type == type)
        return;
    throw RuntimeError(op, "variable type mismatch");
}
