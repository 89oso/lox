#include "script/interpreter.hpp"
#include "common/exception.hpp"
#include "common/finally.hpp"

#include <iostream>
#include <format>

Interpreter::Interpreter() {
    _current_env = &_global_env;
}

void Interpreter::interpret(Node* node) {
    try {
        // TODO: force interpreter to only take in a statement ptr
        node->accept(this);
    } catch (const RuntimeError& e) {
        std::cerr << "[runtime error]: " << e.what() << "\n";
    }
}

void Interpreter::visit_print_stmt(PrintStmt* stmt) {
    auto variable = evaluate(stmt->expr.get());

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
}

void Interpreter::visit_expr_stmt(ExprStmt* stmt) {
    evaluate(stmt->expr.get());
}

void Interpreter::visit_var_stmt(VarStmt* stmt) {
    // value is nil by default (ScriptValue constructor)
    ScriptValue value;
    if (stmt->initializer)
        value = evaluate(stmt->initializer.get());

    _current_env->define_variable(stmt->name.value, value);
}

void Interpreter::visit_block_stmt(BlockStmt* stmt) {
    auto environment = std::make_unique<ScriptEnvironment>(_current_env);
    execute_block(stmt->statements, std::move(environment));
}

void Interpreter::visit_if_stmt(IfStmt* stmt) {
    ScriptValue value = evaluate(stmt->condition.get());

    if (is_true(value))
        execute(stmt->then_branch.get());
    else if (stmt->else_branch)
        execute(stmt->else_branch.get());
}

void Interpreter::visit_while_stmt(WhileStmt* stmt) {
    Node* condition = stmt->condition.get();

    while (is_true(evaluate(condition))) {
        execute(stmt->body.get());
    }
}

void Interpreter::visit_unary_expr(UnaryExpr* node) {
    auto variable = evaluate(node->expr.get());
    if (node->op.type == TokenType::TT_MINUS) {
        assert_value_type(node->op, ScriptValueType::Number, variable);
        variable.value = -std::get<f64>(variable.value);
    } else if (node->op.type == TokenType::TT_BANG) {
        variable.type = ScriptValueType::Boolean;
        variable.value = !is_true(variable);
    }

    _expr_value = variable;
}

void Interpreter::visit_binary_expr(BinaryExpr* node) {
    auto left = evaluate(node->left.get());
    auto right = evaluate(node->right.get());

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
        variable.value = std::get<f64>(left.value) > std::get<f64>(right.value);
        break;
    case TokenType::TT_GREATER_EQUAL:
        assert_values_type(node->op, ScriptValueType::Number, left, right);
        variable.type = ScriptValueType::Boolean;
        variable.value = std::get<f64>(left.value) >= std::get<f64>(right.value);
        break;
    case TokenType::TT_LESS:
        assert_values_type(node->op, ScriptValueType::Number, left, right);
        variable.type = ScriptValueType::Boolean;
        variable.value = std::get<f64>(left.value) < std::get<f64>(right.value);
        break;
    case TokenType::TT_LESS_EQUAL:
        assert_values_type(node->op, ScriptValueType::Number, left, right);
        variable.type = ScriptValueType::Boolean;
        variable.value = std::get<f64>(left.value) <= std::get<f64>(right.value);
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

    _expr_value = variable;
}

void Interpreter::visit_grouping_expr(GroupingExpr* node) {
    evaluate(node->expr.get());
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

    _expr_value = variable;
}

void Interpreter::visit_comma_expr(CommaExpr* node) {
    // TODO
}

void Interpreter::visit_logical_expr(LogicalExpr* node) {
    ScriptValue left_result = evaluate(node->left.get());

    // TODO: modify the top of the stack? prob not needed
    if (node->op.type == TokenType::TT_OR) {
        if (is_true(left_result))
            return;
    } else {
        if (!is_true(left_result))
            return;
    }

    evaluate(node->right.get());
}

void Interpreter::visit_conditional_expr(ConditionalExpr* node) {
    // TODO
}

void Interpreter::visit_variable_expr(VariableExpr* node) {
    _expr_value = _current_env->find_variable(node->name);
}

void Interpreter::visit_assignment_expr(AssignmentExpr* node) {
    auto value = evaluate(node->value.get());
    _current_env->assign_variable(node->name, value);
}

void Interpreter::execute_block(std::vector<Node::ptr>& statements, std::unique_ptr<ScriptEnvironment> environment) {
    ScriptEnvironment* previous_env = _current_env;

    // defer this
    auto _ = oso::finally([&] {
        _current_env = previous_env;
    });

    _current_env = environment.get();
    for (auto& stmt : statements) {
        execute(reinterpret_cast<Stmt*>(stmt.get()));
    }
}

ScriptValue& Interpreter::evaluate(Node* expr) {
    expr->accept(this);
    return _expr_value;
}

void Interpreter::execute(Stmt* stmt) {
    stmt->accept(this);
}

bool Interpreter::is_true(ScriptValue variable) {
    if (variable.type == ScriptValueType::Nil)
        return false;
    else if (variable.type == ScriptValueType::Boolean)
        return std::get<bool>(variable.value);
    return true;
}

bool Interpreter::is_equal(ScriptValue a, ScriptValue b) {
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
