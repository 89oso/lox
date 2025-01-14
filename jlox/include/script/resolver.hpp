#pragma once

#include "interpreter.hpp"

namespace script {

enum class ScopeType {
    Global,
    Function,
};

enum class VariableState {
    Declared,
    Defined,
    Used,
};

class Resolver : public Visitor {
public:
    Resolver(Interpreter* interpreter);

    bool error() const;
    void run(std::vector<Node::ptr>& statements);

    void visit_print_stmt(PrintStmt* stmt) override;
    void visit_expr_stmt(ExprStmt* stmt) override;
    void visit_var_stmt(VarStmt* stmt) override;
    void visit_block_stmt(BlockStmt* stmt) override;
    void visit_if_stmt(IfStmt* stmt) override;
    void visit_while_stmt(WhileStmt* stmt) override;
    void visit_break_stmt(BreakStmt* stmt) override;
    void visit_function_stmt(FunctionStmt* stmt) override;
    void visit_return_stmt(ReturnStmt* stmt) override;
    void visit_class_stmt(ClassStmt* stmt) override;

    void visit_unary_expr(UnaryExpr* node) override;
    void visit_binary_expr(BinaryExpr* node) override;
    void visit_grouping_expr(GroupingExpr* node) override;
    void visit_literal_expr(LiteralExpr* node) override;
    void visit_logical_expr(LogicalExpr* node) override;
    void visit_conditional_expr(ConditionalExpr* node) override;
    void visit_variable_expr(VariableExpr* node) override;
    void visit_assignment_expr(AssignmentExpr* node) override;
    void visit_call_expr(CallExpr* node) override;
    void visit_get_expr(GetExpr* node) override;
    void visit_set_expr(SetExpr* node) override;

private:
    bool _error;
    Interpreter* _interpreter;
    std::vector<std::unordered_map<std::string, VariableState>> _scopes;
    ScopeType _current_scope_type;

    void throw_error(Token& token, const std::string& error);

    void resolve_stmt(Stmt* stmt);
    void resolve_expr(Expr* expr);
    void resolve_statements(std::vector<Node::ptr>& statements);
    void resolve_function(FunctionStmt* stmt, ScopeType scope_type);

    void begin_scope();
    void end_scope();
    void check_unused_variables();

    void declare(Token& name);
    void define(Token& name);

    void resolve_local(Expr* expr, Token& name);
};

} // namespace script
