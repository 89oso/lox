#include "script/ast.hpp"

UnaryExpr::UnaryExpr(TokenType op, Node::ptr expr)
    : op(op),
      expr(std::move(expr)) {
}

void UnaryExpr::accept(Visitor* visitor) {
    visitor->visit(this);
}

BinaryExpr::BinaryExpr(TokenType op, Node::ptr left, Node::ptr right)
    : op(op),
      left(std::move(left)),
      right(std::move(right)) {
}

void BinaryExpr::accept(Visitor* visitor) {
    visitor->visit(this);
}

GroupingExpr::GroupingExpr(Node::ptr expr)
    : expr(std::move(expr)) {
}

void GroupingExpr::accept(Visitor* visitor) {
    visitor->visit(this);
}

LiteralExpr::LiteralExpr(Type type)
    : type(type) {
}

void LiteralExpr::accept(Visitor* visitor) {
    visitor->visit(this);
}

CommaExpr::CommaExpr(std::vector<Node::ptr> expressions)
    : expressions(std::move(expressions)) {
}

void CommaExpr::accept(Visitor* visitor) {
    visitor->visit(this);
}

LogicalExpr::LogicalExpr(TokenType op, Node::ptr left, Node::ptr right)
    : op(op),
      left(std::move(left)),
      right(std::move(right)) {
}

void LogicalExpr::accept(Visitor* visitor) {
    visitor->visit(this);
}

ConditionalExpr::ConditionalExpr(Node::ptr expr, Node::ptr left, Node::ptr right)
    : expr(std::move(expr)),
      left(std::move(left)),
      right(std::move(right)) {
}

void ConditionalExpr::accept(Visitor* visitor) {
    visitor->visit(this);
}
