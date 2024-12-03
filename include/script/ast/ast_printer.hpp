#pragma once

#include "expr.hpp"
#include <iostream>

struct ASTPrinter : public Visitor {
    void print(Node* node) {
        node->accept(this);
        std::cout << "\n";
    }

    void visit(UnaryExpr* node) override {
        std::cout << "(";

        switch (node->op) {
        case TokenType::TT_BANG:
            std::cout << "!";
            break;
        case TokenType::TT_MINUS:
            std::cout << "-";
            break;
        default:
            break;
        }

        std::cout << " ";

        node->expr->accept(this);

        std::cout << ")";
    }

    void visit(BinaryExpr* node) override {
        std::cout << "BinaryExpr(";

        switch (node->op) {
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
        case TokenType::TT_EQUAL_EQUAL:
            std::cout << "==";
            break;
        case TokenType::TT_BANG_EQUAL:
            std::cout << "!=";
            break;
        default:
            break;
        }

        std::cout << " ";
        node->left->accept(this);

        std::cout << " ";
        node->right->accept(this);

        std::cout << ")";
    }

    void visit(GroupingExpr* node) override {
        std::cout << "(";

        std::cout << "group";

        std::cout << " ";

        node->expr->accept(this);

        std::cout << ")";
    }

    void visit(LiteralExpr* node) override {
        std::cout << node->value;
    }
};
