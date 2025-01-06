#include "script/interpreter.hpp"
#include "common/exception.hpp"
#include "common/finally.hpp"

#include <iostream>
#include <fmt/core.h>

Interpreter::Interpreter() {
    _global_env.set_name("global_scope");
    _current_env = &_global_env;
    _control_flow_state = ControlFlowState::None;
}

void Interpreter::interpret(Node* node) {
    try {
        node->accept(this);
    } catch (const RuntimeError& e) {
        std::cerr << "[runtime error]: " << e.what() << "\n";
    }
}

void Interpreter::visit_print_stmt(PrintStmt* stmt) {
    auto variable = evaluate(stmt->expr.get());

    std::cout << "[runtime]: ";

    if (variable.type == ScriptObjectType::Nil) {
        std::cout << "nil";
    } else if (variable.type == ScriptObjectType::Boolean) {
        std::cout << (std::get<bool>(variable.value) ? "true" : "false");
    } else if (variable.type == ScriptObjectType::Number) {
        std::cout << std::get<f64>(variable.value);
    } else if (variable.type == ScriptObjectType::String) {
        std::cout << std::get<std::string>(variable.value);
    } else if (variable.type == ScriptObjectType::Callable) {
        auto callable = std::get<ScriptObject::Callable::ptr>(variable.value);
        std::cout << callable->to_string();
    }

    std::cout << "\n";
}

void Interpreter::visit_expr_stmt(ExprStmt* stmt) {
    evaluate(stmt->expr.get());
}

void Interpreter::visit_var_stmt(VarStmt* stmt) {
    // object is nil by default (ScriptObject constructor)
    ScriptObject object;
    if (stmt->initializer)
        object = evaluate(stmt->initializer.get());

    _current_env->define_variable(stmt->name.value, object);
}

void Interpreter::visit_block_stmt(BlockStmt* stmt) {
    static int counter = 0; // TODO: clean
    if (_control_flow_state != ControlFlowState::None)
        return;

    auto environment = std::make_shared<ScriptEnvironment>(_current_env);
    environment->set_name(fmt::format("block_scope_{}", counter));
    execute_block(stmt->statements, environment);
}

void Interpreter::visit_if_stmt(IfStmt* stmt) {
    ScriptObject object = evaluate(stmt->condition.get());

    if (is_true(object))
        execute(stmt->then_branch.get());
    else if (stmt->else_branch)
        execute(stmt->else_branch.get());
}

void Interpreter::visit_while_stmt(WhileStmt* stmt) {
    Node* condition = stmt->condition.get();

    while (is_true(evaluate(condition))) {
        execute(stmt->body.get());

        if (_control_flow_state == ControlFlowState::Break || _control_flow_state == ControlFlowState::Return) {
            if (_control_flow_state == ControlFlowState::Break) {
                _control_flow_state = ControlFlowState::None;
            }
            break;
        }
    }
}

void Interpreter::visit_break_stmt(BreakStmt* stmt) {
    _control_flow_state = ControlFlowState::Break;
}

void Interpreter::visit_function_stmt(FunctionStmt* stmt) {
    ScriptObject function;
    function.type = ScriptObjectType::Callable;

    auto closure = std::make_shared<ScriptEnvironment>(_current_env);
    function.value = std::make_shared<ScriptFunction>(stmt, closure, stmt->name.type == TT_INVALID);

    if (stmt->name.type == TokenType::TT_INVALID) {
        stmt->name.value = "$anon";
        closure->define_variable(stmt->name.value, function);
    } else {
        _current_env->define_variable(stmt->name.value, function);
    }

    closure->set_name(fmt::format("function_scope_{}", stmt->name.value));

    push_variable(ScriptObjectType::Callable, function);
}

void Interpreter::visit_return_stmt(ReturnStmt* stmt) {
    if (stmt->expr) {
        evaluate(stmt->expr.get());
    }

    _control_flow_state = ControlFlowState::Return;
}

void Interpreter::visit_unary_expr(UnaryExpr* node) {
    auto variable = evaluate(node->expr.get());
    if (node->op.type == TokenType::TT_MINUS) {
        assert_object_type(node->op, ScriptObjectType::Number, variable);
        variable.value = -std::get<f64>(variable.value);
    } else if (node->op.type == TokenType::TT_BANG) {
        variable.type = ScriptObjectType::Boolean;
        variable.value = !is_true(variable);
    }

    push_variable(variable.type, variable);
}

void Interpreter::visit_binary_expr(BinaryExpr* node) {
    auto left = evaluate(node->left.get());
    auto right = evaluate(node->right.get());

    ScriptObject variable;

    switch (node->op.type) {
    case TokenType::TT_PLUS: {
        if (left.type == ScriptObjectType::Number) {
            if (right.type == ScriptObjectType::String) {
                variable.type = ScriptObjectType::String;
                variable.value = fmt::format("{}", std::get<f64>(left.value)) + std::get<std::string>(right.value);
                break;
            }
            assert_object_type(node->op, ScriptObjectType::Number, right);
            variable.type = ScriptObjectType::Number;
            variable.value = std::get<f64>(left.value) + std::get<f64>(right.value);
        } else if (left.type == ScriptObjectType::String) {
            if (right.type == ScriptObjectType::Number) {
                variable.type = ScriptObjectType::String;
                variable.value = std::get<std::string>(left.value) + fmt::format("{}", std::get<f64>(right.value));
                break;
            }
            assert_object_type(node->op, ScriptObjectType::String, right);
            variable.type = ScriptObjectType::String;
            variable.value = std::get<std::string>(left.value) + std::get<std::string>(right.value);
        } else {
            throw RuntimeError(node->op, "only numbers and strings are allowed for binary expressions");
            return;
        }
    } break;
    case TokenType::TT_MINUS:
        assert_objects_type(node->op, ScriptObjectType::Number, left, right);
        variable.type = ScriptObjectType::Number;
        variable.value = std::get<f64>(left.value) - std::get<f64>(right.value);
        break;
    case TokenType::TT_STAR:
        assert_objects_type(node->op, ScriptObjectType::Number, left, right);
        variable.type = ScriptObjectType::Number;
        variable.value = std::get<f64>(left.value) * std::get<f64>(right.value);
        break;
    case TokenType::TT_SLASH: {
        assert_objects_type(node->op, ScriptObjectType::Number, left, right);
        f64 lhs_value = std::get<f64>(left.value);
        f64 rhs_value = std::get<f64>(right.value);
        if (lhs_value == 0.0 || rhs_value == 0.0) {
            throw RuntimeError(node->op, "division by zero is not allowed");
            return;
        }
        variable.type = ScriptObjectType::Number;
        variable.value = lhs_value / rhs_value;
    } break;
    case TokenType::TT_GREATER:
        assert_objects_type(node->op, ScriptObjectType::Number, left, right);
        variable.type = ScriptObjectType::Boolean;
        variable.value = std::get<f64>(left.value) > std::get<f64>(right.value);
        break;
    case TokenType::TT_GREATER_EQUAL:
        assert_objects_type(node->op, ScriptObjectType::Number, left, right);
        variable.type = ScriptObjectType::Boolean;
        variable.value = std::get<f64>(left.value) >= std::get<f64>(right.value);
        break;
    case TokenType::TT_LESS:
        assert_objects_type(node->op, ScriptObjectType::Number, left, right);
        variable.type = ScriptObjectType::Boolean;
        variable.value = std::get<f64>(left.value) < std::get<f64>(right.value);
        break;
    case TokenType::TT_LESS_EQUAL:
        assert_objects_type(node->op, ScriptObjectType::Number, left, right);
        variable.type = ScriptObjectType::Boolean;
        variable.value = std::get<f64>(left.value) <= std::get<f64>(right.value);
        break;
    case TokenType::TT_BANG_EQUAL:
        variable.type = ScriptObjectType::Boolean;
        variable.value = !is_equal(left, right);
        break;
    case TokenType::TT_EQUAL_EQUAL:
        variable.type = ScriptObjectType::Boolean;
        variable.value = is_equal(left, right);
        break;
    default:
        throw RuntimeError(node->op, "unknown binary expression operator");
        break;
    }

    push_variable(variable.type, variable);
}

void Interpreter::visit_grouping_expr(GroupingExpr* node) {
    evaluate(node->expr.get());
}

// TODO: strings currently come from the token created by the lexer (std::string)
//     : maybe an arena for the whole AST would be a good idea?
void Interpreter::visit_literal_expr(LiteralExpr* node) {
    ScriptObject variable;

    if (node->literal_type == LiteralExpr::LiteralType::Nil) {
        variable.type = ScriptObjectType::Nil;
    } else if (node->literal_type == LiteralExpr::LiteralType::Boolean) {
        variable.type = ScriptObjectType::Boolean;
        variable.value = std::get<bool>(node->value);
    } else if (node->literal_type == LiteralExpr::LiteralType::Number) {
        variable.type = ScriptObjectType::Number;
        variable.value = std::get<f64>(node->value);
    } else if (node->literal_type == LiteralExpr::LiteralType::String) {
        variable.type = ScriptObjectType::String;
        variable.value = std::get<std::string>(node->value);
    }

    push_variable(variable.type, variable);
}

void Interpreter::visit_logical_expr(LogicalExpr* node) {
    ScriptObject left_result = evaluate(node->left.get());

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
    auto result = evaluate(node->expr.get());

    if (is_true(result))
        evaluate(node->left.get());
    else
        evaluate(node->right.get());
}

void Interpreter::visit_variable_expr(VariableExpr* node) {
    auto result = _current_env->find_variable(node->name);
    push_variable(result.type, result);
}

void Interpreter::visit_assignment_expr(AssignmentExpr* node) {
    auto value = evaluate(node->value.get());
    _current_env->assign_variable(node->name, value);
}

void Interpreter::visit_call_expr(CallExpr* node) {
    ScriptObject callee = evaluate(node->callee.get());
    if (callee.type != ScriptObjectType::Callable) {
        throw RuntimeError(node->paren, "Can only call functions and classes");
    }

    std::vector<ScriptObject> arguments;
    for (auto& arg : node->arguments) {
        arguments.push_back(evaluate(arg.get()));
    }

    auto callable = std::get<ScriptObject::Callable::ptr>(callee.value);

    if (arguments.size() != callable->arity) {
        throw RuntimeError(node->paren,
                           fmt::format("Expected {0} arguments but got {1}.", callable->arity, arguments.size()));
    }

    auto result = callable->call(this, arguments);
    push_variable(result.type, result);
}

ScriptEnvironment& Interpreter::global_env() {
    return _global_env;
}

ControlFlowState Interpreter::control_flow_state() {
    return _control_flow_state;
}

void Interpreter::set_control_flow_state(ControlFlowState state) {
    _control_flow_state = state;
}

ScriptObject& Interpreter::expr_result() {
    return _expr_result;
}

ScriptObject Interpreter::evaluate(Node* expr) {
    expr->accept(this);
    return _expr_result;
}

void Interpreter::execute(Stmt* stmt) {
    stmt->accept(this);
}

void Interpreter::execute_block(std::vector<Node::ptr>& statements, std::shared_ptr<ScriptEnvironment> environment) {
    ScriptEnvironment* previous_env = _current_env;

    // defer this
    auto _ = oso::finally([&] {
        _current_env = previous_env;
    });

    _current_env = environment.get();
    for (auto& stmt : statements) {
        execute(reinterpret_cast<Stmt*>(stmt.get()));
        if (_control_flow_state != ControlFlowState::None)
            break;
    }
}

void Interpreter::push_variable(u8 type, ScriptObject& value) {
    if (type != value.type) {
        // TODO: runtime error
    }

    _expr_result = value;
}

bool Interpreter::is_true(ScriptObject variable) {
    if (variable.type == ScriptObjectType::Nil)
        return false;
    else if (variable.type == ScriptObjectType::Boolean)
        return std::get<bool>(variable.value);
    return true;
}

bool Interpreter::is_equal(ScriptObject a, ScriptObject b) {
    if (a.type != b.type)
        return false;
    else if (a.type == ScriptObjectType::Nil && b.type == ScriptObjectType::Nil)
        return true;
    else if (a.type == ScriptObjectType::Nil)
        return false;
    else if (a.type == ScriptObjectType::Number)
        return std::get<f64>(a.value) == std::get<f64>(b.value);
    else if (a.type == ScriptObjectType::String)
        return std::get<std::string>(a.value) == std::get<std::string>(b.value);
    return false;
}

void Interpreter::assert_object_type(Token& op, ScriptObjectType type, ScriptObject& variable) {
    if (variable.type == type)
        return;
    throw RuntimeError(op, "variable type mismatch");
}

void Interpreter::assert_objects_type(Token& op, ScriptObjectType type, ScriptObject& a, ScriptObject& b) {
    if (a.type == type && b.type == type)
        return;
    throw RuntimeError(op, "variable type mismatch");
}
