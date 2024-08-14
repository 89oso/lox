#pragma once

#include "../token.hpp"

namespace lox::ast {
    class Expr {
    };

    class ExprUnary : public Expr {
    public:
        ExprUnary(Token op, Expr right) : op(op), right(right) {
        }

        Token op;
        Expr right;
    };

    class ExprBinary : public Expr {
    public:
        ExprBinary(Expr left, Expr right, Token op) : left(left), right(right), op(op) {
        }

        Expr left;
        Expr right;
        Token op;
    };

    class ExprGrouping : public Expr {
    public:
        ExprGrouping(Expr expression) : expression(expression) {
        }

        Expr expression;
    };

    class ExprLiteral : public Expr {
    public:
        ExprLiteral(std::string value) : value(value) {
        }

        std::string value;
    };
}
