#pragma once

#include "common/common.hpp"
#include "script/ast.hpp"

#include <functional>
#include <variant>

enum ScriptObjectType : u8 {
    Nil,
    Boolean,
    Number,
    String,
    Callable,
};

struct Interpreter;

struct ScriptObject {
    using callable_type = std::function<ScriptObject(Interpreter* interpreter, std::vector<ScriptObject>& arguments)>;

    ScriptObject();

    struct Callable {
        Callable();
        explicit Callable(u16 arity, callable_type function);

        virtual ScriptObject call(Interpreter* interpreter, std::vector<ScriptObject>& arguments);

        u16 arity;
        callable_type function;
    };

    u8 type;
    std::variant<bool, double, std::string, std::shared_ptr<Callable>> value;
};

struct ScriptFunction : ScriptObject::Callable {
    ScriptFunction(FunctionStmt* decl);
    ScriptObject call(Interpreter* interpreter, std::vector<ScriptObject>& arguments) override;

    FunctionStmt* decl;
};
