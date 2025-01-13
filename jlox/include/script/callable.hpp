#pragma once

#include "common/common.hpp"
#include "script/object.hpp"

#include <functional>
#include <vector>

namespace script {

enum ScriptCallableType : u8 {
    Invalid,
    Builtin,
    Function,
};

class Interpreter;

struct ScriptCallable {
    using function_type = std::function<ScriptObject(Interpreter* interpreter, std::vector<ScriptObject>& arguments)>;

    u8 type;
    u16 arity;
    function_type function;

    ScriptCallable();
    ScriptCallable(u16 arity, function_type& function);

    virtual ScriptObject call(Interpreter* interpreter, std::vector<ScriptObject>& arguments);
    virtual std::string to_string();
};

} // namespace script
