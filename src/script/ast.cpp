#include "script/ast.hpp"

UnaryExpr::UnaryExpr(Token op, Node::ptr expr)
    : op(op),
      expr(std::move(expr)) {
}

void UnaryExpr::accept(Visitor* visitor) {
    visitor->visit_unary_expr(this);
}

BinaryExpr::BinaryExpr(Token op, Node::ptr left, Node::ptr right)
    : op(op),
      left(std::move(left)),
      right(std::move(right)) {
}

void BinaryExpr::accept(Visitor* visitor) {
    visitor->visit_binary_expr(this);
}

GroupingExpr::GroupingExpr(Node::ptr expr)
    : expr(std::move(expr)) {
}

void GroupingExpr::accept(Visitor* visitor) {
    visitor->visit_grouping_expr(this);
}

LiteralExpr::LiteralExpr(LiteralType literal_type)
    : literal_type(literal_type) {
}

void LiteralExpr::accept(Visitor* visitor) {
    visitor->visit_literal_expr(this);
}

CommaExpr::CommaExpr(std::vector<Node::ptr> expressions)
    : expressions(std::move(expressions)) {
}

void CommaExpr::accept(Visitor* visitor) {
    visitor->visit_comma_expr(this);
}

LogicalExpr::LogicalExpr(Token op, Node::ptr left, Node::ptr right)
    : op(op),
      left(std::move(left)),
      right(std::move(right)) {
}

void LogicalExpr::accept(Visitor* visitor) {
    visitor->visit_logical_expr(this);
}

ConditionalExpr::ConditionalExpr(Node::ptr expr, Node::ptr left, Node::ptr right)
    : expr(std::move(expr)),
      left(std::move(left)),
      right(std::move(right)) {
}

void ConditionalExpr::accept(Visitor* visitor) {
    visitor->visit_conditional_expr(this);
}

VariableExpr::VariableExpr(Token name)
    : name(name) {
}

void VariableExpr::accept(Visitor* visitor) {
    visitor->visit_variable_expr(this);
}

AssignmentExpr::AssignmentExpr(Token name, Node::ptr value)
    : name(name),
      value(std::move(value)) {
}

void AssignmentExpr::accept(Visitor* visitor) {
    visitor->visit_assignment_expr(this);
}

PrintStmt::PrintStmt(Node::ptr expr)
    : expr(std::move(expr)) {
}

void PrintStmt::accept(Visitor* visitor) {
    visitor->visit_print_stmt(this);
}

ExprStmt::ExprStmt(Node::ptr expr)
    : expr(std::move(expr)) {
}

void ExprStmt::accept(Visitor* visitor) {
    visitor->visit_expr_stmt(this);
}

VarStmt::VarStmt(Token name, Expr::ptr initializer)
    : name(name),
      initializer(std::move(initializer)) {
}

void VarStmt::accept(Visitor* visitor) {
    visitor->visit_var_stmt(this);
}

BlockStmt::BlockStmt(std::vector<Node::ptr> statements)
    : statements(std::move(statements)) {
}

void BlockStmt::accept(Visitor* visitor) {
    visitor->visit_block_stmt(this);
}

IfStmt::IfStmt(Expr::ptr condition, Stmt::ptr then_branch, Stmt::ptr else_branch)
    : condition(std::move(condition)),
      then_branch(std::move(then_branch)),
      else_branch(std::move(else_branch)) {
}

void IfStmt::accept(Visitor* visitor) {
    visitor->visit_if_stmt(this);
}
