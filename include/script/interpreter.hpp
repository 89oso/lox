#pragma once

#include "ast.hpp"
#include "script_variable.hpp"

class Interpreter : public Visitor {
public:
    Interpreter();

    void visit(UnaryExpr* node) override;
    void visit(BinaryExpr* node) override;
    void visit(GroupingExpr* node) override;
    void visit(LiteralExpr* node) override;
    void visit(CommaExpr* node) override;
    void visit(LogicalExpr* node) override;
    void visit(ConditionalExpr* node) override;

    void interpret(Node* node);

private:
    std::vector<ScriptVariable> variable_stack;

    void print_stack();

    bool is_true(ScriptVariable& variable);
    bool is_equal(ScriptVariable& a, ScriptVariable& b);

    void assert_variable_type(Token op, ScriptVariableType type, ScriptVariable& variable);
    void assert_variables_type(Token op, ScriptVariableType type, ScriptVariable& a, ScriptVariable& b);
};
