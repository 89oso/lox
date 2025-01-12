#pragma once

#include <memory>
#include <variant>
#include <vector>

#include "token.hpp"

struct UnaryExpr;
struct BinaryExpr;
struct GroupingExpr;
struct LiteralExpr;
struct LogicalExpr;
struct ConditionalExpr;
struct VariableExpr;
struct AssignmentExpr;
struct CallExpr;

struct PrintStmt;
struct ExprStmt;
struct VarStmt;
struct BlockStmt;
struct IfStmt;
struct WhileStmt;
struct BreakStmt;
struct FunctionStmt;
struct ReturnStmt;
struct ClassStmt;

class Visitor {
public:
    virtual void visit_print_stmt(PrintStmt* stmt) = 0;
    virtual void visit_expr_stmt(ExprStmt* stmt) = 0;
    virtual void visit_var_stmt(VarStmt* stmt) = 0;
    virtual void visit_block_stmt(BlockStmt* stmt) = 0;
    virtual void visit_if_stmt(IfStmt* stmt) = 0;
    virtual void visit_while_stmt(WhileStmt* stmt) = 0;
    virtual void visit_break_stmt(BreakStmt* stmt) = 0;
    virtual void visit_function_stmt(FunctionStmt* stmt) = 0;
    virtual void visit_return_stmt(ReturnStmt* stmt) = 0;
    virtual void visit_class_stmt(ClassStmt* stmt) = 0;

    virtual void visit_unary_expr(UnaryExpr* node) = 0;
    virtual void visit_binary_expr(BinaryExpr* node) = 0;
    virtual void visit_grouping_expr(GroupingExpr* node) = 0;
    virtual void visit_literal_expr(LiteralExpr* node) = 0;
    virtual void visit_logical_expr(LogicalExpr* node) = 0;
    virtual void visit_conditional_expr(ConditionalExpr* node) = 0;
    virtual void visit_variable_expr(VariableExpr* node) = 0;
    virtual void visit_assignment_expr(AssignmentExpr* node) = 0;
    virtual void visit_call_expr(CallExpr* node) = 0;
};

struct Node {
    using ptr = std::unique_ptr<Node>;

    enum Type {
        UnaryExpr,
        BinaryExpr,
        GroupingExpr,
        LiteralExpr,
        LogicalExpr,
        ConditionalExpr,
        VariableExpr,
        AssignmentExpr,
        CallExpr,
        PrintStmt,
        ExprStmt,
        VarStmt,
        BlockStmt,
        IfStmt,
        WhileStmt,
        BreakStmt,
        FunctionStmt,
        ReturnStmt,
        ClassStmt,
    };

    virtual ~Node() = default;
    virtual void accept(Visitor* visitor) = 0;
    virtual u8 type() = 0;
};

struct Expr : public Node {
    virtual ~Expr() = default;
    virtual void accept(Visitor* visitor) = 0;
};

struct UnaryExpr : Expr {
    explicit UnaryExpr(Token& op, Node::ptr expr);
    void accept(Visitor* visitor) override;

    u8 type() override {
        return Node::Type::UnaryExpr;
    }

    Token op;
    Node::ptr expr;
};

struct BinaryExpr : Expr {
    explicit BinaryExpr(Token& op, Node::ptr left, Node::ptr right);
    void accept(Visitor* visitor) override;

    u8 type() override {
        return Node::Type::BinaryExpr;
    }

    Token op;
    Node::ptr left;
    Node::ptr right;
};

struct GroupingExpr : Expr {
    explicit GroupingExpr(Node::ptr expr);
    void accept(Visitor* visitor) override;

    u8 type() override {
        return Node::Type::GroupingExpr;
    }

    Node::ptr expr;
};

struct LiteralExpr : Expr {
    enum class LiteralType { Nil, Boolean, Number, String };

    explicit LiteralExpr(LiteralType literal_type);
    void accept(Visitor* visitor) override;

    u8 type() override {
        return Node::Type::LiteralExpr;
    }

    LiteralType literal_type;
    std::variant<bool, f64, std::string> value;
};

struct LogicalExpr : Expr {
    explicit LogicalExpr(Token& op, Node::ptr left, Node::ptr right);
    void accept(Visitor* visitor) override;

    u8 type() override {
        return Node::Type::LogicalExpr;
    }

    Token op;
    Node::ptr left;
    Node::ptr right;
};

struct ConditionalExpr : Expr {
    explicit ConditionalExpr(Node::ptr expr, Node::ptr left, Node::ptr right);
    void accept(Visitor* visitor) override;

    u8 type() override {
        return Node::Type::ConditionalExpr;
    }

    Node::ptr expr;
    Node::ptr left;
    Node::ptr right;
};

struct VariableExpr : Expr {
    explicit VariableExpr(Token& name);
    void accept(Visitor* visitor) override;

    u8 type() override {
        return Node::Type::VariableExpr;
    }

    Token name;
};

struct AssignmentExpr : Expr {
    explicit AssignmentExpr(Token& name, Node::ptr value);
    void accept(Visitor* visitor) override;

    u8 type() override {
        return Node::Type::AssignmentExpr;
    }

    Token name;
    Node::ptr value;
};

struct CallExpr : Expr {
    explicit CallExpr(Node::ptr callee, Token& paren, std::vector<Node::ptr> arguments);
    void accept(Visitor* visitor) override;

    u8 type() override {
        return Node::Type::CallExpr;
    }

    Node::ptr callee;
    Token paren;
    std::vector<Node::ptr> arguments;
};

struct Stmt : public Node {
    using ptr = std::unique_ptr<Stmt>;

    virtual ~Stmt() = default;
    virtual void accept(Visitor* visitor) = 0;
};

struct PrintStmt : Stmt {
    explicit PrintStmt(Node::ptr expr);
    void accept(Visitor* visitor) override;

    u8 type() override {
        return Node::Type::PrintStmt;
    }

    Node::ptr expr;
};

struct ExprStmt : Stmt {
    explicit ExprStmt(Node::ptr expr);
    void accept(Visitor* visitor) override;

    u8 type() override {
        return Node::Type::ExprStmt;
    }

    Node::ptr expr;
};

struct VarStmt : Stmt {
    explicit VarStmt(Token& name, Node::ptr initializer);
    void accept(Visitor* visitor) override;

    u8 type() override {
        return Node::Type::VarStmt;
    }

    Token name;
    Node::ptr initializer;
};

struct BlockStmt : Stmt {
    explicit BlockStmt(std::vector<Node::ptr> statements);
    void accept(Visitor* visitor) override;

    u8 type() override {
        return Node::Type::BlockStmt;
    }

    std::vector<Node::ptr> statements;
};

struct IfStmt : Stmt {
    explicit IfStmt(Expr::ptr condition, Stmt::ptr then_branch, Stmt::ptr else_branch);
    void accept(Visitor* visitor) override;

    u8 type() override {
        return Node::Type::IfStmt;
    }

    Expr::ptr condition;
    Stmt::ptr then_branch;
    Stmt::ptr else_branch;
};

struct WhileStmt : Stmt {
    explicit WhileStmt(Expr::ptr condition, Stmt::ptr body);
    void accept(Visitor* visitor) override;

    u8 type() override {
        return Node::Type::WhileStmt;
    }

    Expr::ptr condition;
    Stmt::ptr body;
};

struct BreakStmt : Stmt {
    explicit BreakStmt();
    void accept(Visitor* visitor) override;

    u8 type() override {
        return Node::Type::BreakStmt;
    }
};

struct FunctionStmt : Stmt {
    explicit FunctionStmt(Token& name, std::vector<Token> params, std::vector<Node::ptr> body);
    void accept(Visitor* visitor) override;

    u8 type() override {
        return Node::Type::FunctionStmt;
    }

    Token name;
    std::vector<Token> params;
    std::vector<Node::ptr> body;
};

struct ReturnStmt : Stmt {
    explicit ReturnStmt(Token& keyword, Node::ptr expr);
    void accept(Visitor* visitor) override;

    u8 type() override {
        return Node::Type::ReturnStmt;
    }

    Token keyword;
    Node::ptr expr;
};

struct ClassStmt : Stmt {
    explicit ClassStmt(Token& name, std::vector<Node::ptr> functions);
    void accept(Visitor* visitor) override;

    u8 type() override {
        return Node::Type::ClassStmt;
    }

    Token name;
    std::vector<Node::ptr> functions;
};
