#pragma once

#include "common/common.hpp"
#include "script/ast.hpp"

#include <functional>
#include <variant>

enum ScriptValueType : u8 {
    Nil,
    Boolean,
    Number,
    String,
    Callable,
};

struct Interpreter;
struct ScriptValue;

struct ScriptCallable {
    using function_type = std::function<void(Interpreter* interpreter, std::vector<ScriptValue>& arguments)>;

    explicit ScriptCallable(u16 arity, function_type function)
        : arity(0),
          function(function) {
    }

    void call(Interpreter* interpreter, std::vector<ScriptValue>& arguments) {
        function(interpreter, arguments);
    }

    // std::string name;
    u16 arity;
    function_type function;
};

struct ScriptValue {
    ScriptValue()
        : type(ScriptValueType::Nil) {
    }

    u8 type;
    std::variant<bool, double, std::string, ScriptCallable> value;
};
