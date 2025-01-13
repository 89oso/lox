#pragma once

#include "ast.hpp"
#include "environment.hpp"

namespace script {

enum class ControlFlowState {
    None,
    Break,
    Return,
};

class Interpreter : public Visitor {
public:
    Interpreter();

    void interpret(Node* node);
    void resolve(Expr* expr, usize depth);

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

    ScriptEnvironment& global_env();

    ControlFlowState control_flow_state();
    void set_control_flow_state(ControlFlowState state);

    ScriptObject& expr_result();

    void execute_block(std::vector<Node::ptr>& statements, std::shared_ptr<ScriptEnvironment> environment);

private:
    ScriptEnvironment _global_env;
    ScriptEnvironment* _current_env;
    ScriptObject _expr_result;
    ControlFlowState _control_flow_state;
    std::unordered_map<Expr*, usize> _locals;

    // TODO: change places that use Node for type when Expr should be explicitly stated
    ScriptObject evaluate(Node* expr);
    void execute(Stmt* stmt);
    void push_variable(u8 type, ScriptObject& value);
    ScriptObject lookup_variable(Token& name, Expr* expr);

    bool is_true(ScriptObject object);
    bool is_equal(ScriptObject a, ScriptObject b);

    void assert_object_type(Token& op, ScriptObjectType type, ScriptObject& object);
    void assert_objects_type(Token& op, ScriptObjectType type, ScriptObject& a, ScriptObject& b);
};

} // namespace script
