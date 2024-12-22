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
        using ptr = std::shared_ptr<Callable>;

        Callable();
        explicit Callable(u16 arity, callable_type function);

        virtual ScriptObject call(Interpreter* interpreter, std::vector<ScriptObject>& arguments);
        virtual std::string to_string();

        u16 arity;
        callable_type function;
    };

    u8 type;
    std::variant<bool, double, std::string, Callable::ptr> value;
};

struct ScriptEnvironment;

struct ScriptFunction : ScriptObject::Callable {
    ScriptFunction(FunctionStmt* decl, std::shared_ptr<ScriptEnvironment> closure);
    ScriptObject call(Interpreter* interpreter, std::vector<ScriptObject>& arguments) override;
    std::string to_string() override;

    FunctionStmt* decl;
    std::shared_ptr<ScriptEnvironment> closure;
};
