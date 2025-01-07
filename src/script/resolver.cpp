#include "script/resolver.hpp"
#include "common/exception.hpp"

#include <iostream>

Resolver::Resolver(Interpreter* interpreter)
    : _error(false),
      _interpreter(interpreter),
      _current_scope_type(ScopeType::Global) {
}

bool Resolver::error() const {
    return _error;
}

void Resolver::run(std::vector<Node::ptr>& statements) {
    try {
        resolve_statements(statements);
    } catch (const ResolverError& e) {
        std::cerr << "[resolver error]: " << e.what() << "\n";
    }
}

void Resolver::visit_print_stmt(PrintStmt* stmt) {
    resolve_expr(reinterpret_cast<Expr*>(stmt->expr.get()));
}

void Resolver::visit_expr_stmt(ExprStmt* stmt) {
    resolve_expr(reinterpret_cast<Expr*>(stmt->expr.get()));
}

void Resolver::visit_var_stmt(VarStmt* stmt) {
    declare(stmt->name);
    if (stmt->initializer) {
        resolve_expr(reinterpret_cast<Expr*>(stmt->initializer.get()));
    }
    define(stmt->name);
}

void Resolver::visit_block_stmt(BlockStmt* stmt) {
    begin_scope();
    resolve_statements(stmt->statements);
    end_scope();
}

void Resolver::visit_if_stmt(IfStmt* stmt) {
    resolve_expr(reinterpret_cast<Expr*>(stmt->condition.get()));
    resolve_stmt(stmt->then_branch.get());
    if (stmt->else_branch) {
        resolve_stmt(stmt->else_branch.get());
    }
}

void Resolver::visit_while_stmt(WhileStmt* stmt) {
    resolve_expr(reinterpret_cast<Expr*>(stmt->condition.get()));
    resolve_stmt(reinterpret_cast<Stmt*>(stmt->body.get()));
}

void Resolver::visit_break_stmt(BreakStmt* stmt) {
    (void)stmt;
}

// TODO: make sure this works with anonymous functions too
void Resolver::visit_function_stmt(FunctionStmt* stmt) {
    declare(stmt->name);
    define(stmt->name);
    resolve_function(stmt, ScopeType::Function);
}

void Resolver::visit_return_stmt(ReturnStmt* stmt) {
    if (_current_scope_type == ScopeType::Global) {
        throw_error(stmt->keyword, "Can't return from global scope");
    }

    if (stmt->expr) {
        resolve_expr(reinterpret_cast<Expr*>(stmt->expr.get()));
    }
}

void Resolver::visit_unary_expr(UnaryExpr* node) {
    resolve_expr(reinterpret_cast<Expr*>(node->expr.get()));
}

void Resolver::visit_binary_expr(BinaryExpr* node) {
    resolve_expr(reinterpret_cast<Expr*>(node->left.get()));
    resolve_expr(reinterpret_cast<Expr*>(node->right.get()));
}

void Resolver::visit_grouping_expr(GroupingExpr* node) {
    resolve_expr(reinterpret_cast<Expr*>(node->expr.get()));
}

void Resolver::visit_literal_expr(LiteralExpr* node) {
    (void)node;
}

void Resolver::visit_logical_expr(LogicalExpr* node) {
    resolve_expr(reinterpret_cast<Expr*>(node->left.get()));
    resolve_expr(reinterpret_cast<Expr*>(node->right.get()));
}

void Resolver::visit_conditional_expr(ConditionalExpr* node) {
    resolve_expr(reinterpret_cast<Expr*>(node->expr.get()));
    resolve_expr(reinterpret_cast<Expr*>(node->left.get()));
    resolve_expr(reinterpret_cast<Expr*>(node->right.get()));
}

void Resolver::visit_variable_expr(VariableExpr* node) {
    // check if the variable exists in the current scope and ensure the entry is defined (false means declared)
    if (!_scopes.empty()) {
        auto& scope = _scopes.back();
        if (scope.contains(node->name.value)) {
            auto& variable_state = scope[node->name.value];
            if (variable_state == VariableState::Declared) {
                throw_error(node->name, "Can't read variable in it's own initializer");
            }
        }
    }

    resolve_local(node, node->name);
}

void Resolver::visit_assignment_expr(AssignmentExpr* node) {
    resolve_expr(reinterpret_cast<Expr*>(node->value.get()));
    resolve_local(node, node->name);
}

void Resolver::visit_call_expr(CallExpr* node) {
    resolve_expr(reinterpret_cast<Expr*>(node->callee.get()));

    for (auto& arg : node->arguments) {
        if (arg->type() == Node::Type::FunctionStmt) {
            resolve_stmt(reinterpret_cast<Stmt*>(arg.get()));
        } else {
            resolve_expr(reinterpret_cast<Expr*>(arg.get()));
        }
    }
}

void Resolver::throw_error(Token& token, const std::string& error) {
    _error = true;

    std::string message;
    if (token.type == TokenType::TT_EOF)
        message = error + " at EOF (line " + std::to_string(token.line) + ")";
    else
        message = error + " at '" + token.to_string() + "' (line " + std::to_string(token.line) + ")";

    throw ResolverError(message);
}

void Resolver::resolve_stmt(Stmt* stmt) {
    stmt->accept(this);
}

void Resolver::resolve_expr(Expr* expr) {
    expr->accept(this);
}

void Resolver::resolve_statements(std::vector<Node::ptr>& statements) {
    for (auto& stmt : statements) {
        resolve_stmt(reinterpret_cast<Stmt*>(stmt.get()));
    }
}

// TODO: make sure this works with anonymous functions too
void Resolver::resolve_function(FunctionStmt* stmt, ScopeType scope_type) {
    ScopeType enclosing_scope_type = _current_scope_type;
    _current_scope_type = scope_type;

    begin_scope();

    for (auto& param : stmt->params) {
        declare(param);
        define(param);
    }

    resolve_statements(stmt->body);

    end_scope();

    _current_scope_type = enclosing_scope_type;
}

void Resolver::begin_scope() {
    std::unordered_map<std::string, VariableState> new_scope;
    _scopes.push_back(new_scope);
}

void Resolver::end_scope() {
    auto& scope = _scopes.back();
    for (auto& [name, state] : scope) {
        if (state != VariableState::Used) {
            std::cerr << "[resolver warning]: unused variable '" << name << "'\n";
        }
    }

    _scopes.pop_back();
}

void Resolver::declare(Token& name) {
    if (_scopes.empty()) {
        return;
    }

    auto& scope = _scopes.back();

    if (scope.contains(name.value)) {
        throw_error(name, "Already a variable with this name in current scope");
    }

    scope[name.value] = VariableState::Declared;
}

void Resolver::define(Token& name) {
    if (_scopes.empty()) {
        return;
    }

    auto& scope = _scopes.back();
    scope[name.value] = VariableState::Defined;
}

void Resolver::resolve_local(Expr* expr, Token& name) {
    usize depth = _scopes.size() - 1;
    for (auto scope = _scopes.rbegin(); scope != _scopes.rend(); scope++) {
        if (scope->contains(name.value)) {
            scope->at(name.value) = VariableState::Used;
            _interpreter->resolve(expr, _scopes.size() - 1 - depth);
            return;
        }
        depth--;
    }
}
