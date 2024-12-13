#include "script/ast.hpp"

UnaryExpr::UnaryExpr(TokenType op, Node::ptr expr)
    : op(op),
      expr(std::move(expr)) {
}

void UnaryExpr::accept(Visitor* visitor) {
    visitor->visit_unary_expr(this);
}

BinaryExpr::BinaryExpr(TokenType op, Node::ptr left, Node::ptr right)
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

LiteralExpr::LiteralExpr(Type type)
    : type(type) {
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

LogicalExpr::LogicalExpr(TokenType op, Node::ptr left, Node::ptr right)
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
