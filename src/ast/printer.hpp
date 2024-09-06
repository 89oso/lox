#pragma once

#include "expr.hpp"
#include <iostream>

// BinaryExpr* expr = new BinaryExpr;
// expr->op = TokenType::TT_STAR;
//
// UnaryExpr* left = new UnaryExpr;
// left->op = TokenType::TT_MINUS;
// left->right = new LiteralExpr("123");
// expr->left = left;
//
// GroupingExpr* right = new GroupingExpr;
// right->expression = new LiteralExpr("45.67");
// expr->right = right;
//
// Printer p;
// p.print(expr);

struct Printer : public Visitor {
    void print(Expr* expr) {
        expr->accept(this);
    }

    void visit(UnaryExpr* expr) override {
        std::cout << "(";

        switch (expr->op) {
        case TokenType::TT_PLUS:
            std::cout << "+";
            break;
        case TokenType::TT_MINUS:
            std::cout << "-";
            break;
        case TokenType::TT_STAR:
            std::cout << "*";
            break;
        case TokenType::TT_SLASH:
            std::cout << "/";
            break;
        default:
            break;
        }

        std::cout << " ";

        expr->right->accept(this);

        std::cout << ")";
    }

    void visit(BinaryExpr* expr) override {
        std::cout << "(";

        switch (expr->op) {
        case TokenType::TT_PLUS:
            std::cout << "+";
            break;
        case TokenType::TT_MINUS:
            std::cout << "-";
            break;
        case TokenType::TT_STAR:
            std::cout << "*";
            break;
        case TokenType::TT_SLASH:
            std::cout << "/";
            break;
        default:
            break;
        }

        std::cout << " ";
        expr->left->accept(this);

        std::cout << " ";
        expr->right->accept(this);

        std::cout << ")";
    }

    void visit(GroupingExpr* expr) override {
        std::cout << "(";

        std::cout << "group";

        std::cout << " ";

        expr->expression->accept(this);

        std::cout << ")";
    }

    void visit(LiteralExpr* expr) override {
        std::cout << expr->value;
    }
};
