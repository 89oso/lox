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

struct ScriptValue {
    using callable_function_type =
        std::function<ScriptValue(Interpreter* interpreter, std::vector<ScriptValue>& arguments)>;

    ScriptValue()
        : type(ScriptValueType::Nil) {
    }

    struct Callable {

        explicit Callable(u16 arity, callable_function_type function)
            : arity(0),
              function(function) {
        }

        ScriptValue call(Interpreter* interpreter, std::vector<ScriptValue>& arguments) {
            return function(interpreter, arguments);
        }

        u16 arity;
        callable_function_type function;
    };

    u8 type;
    std::variant<bool, double, std::string, Callable> value;
};
