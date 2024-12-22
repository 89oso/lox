#pragma once

#include "token.hpp"
#include "object.hpp"

#include <unordered_map>

class ScriptEnvironment {
public:
    ScriptEnvironment();
    ScriptEnvironment(ScriptEnvironment* enclosing);

    void assign_variable(const Token& name, ScriptObject& value);
    void define_variable(const std::string& name, ScriptObject& value);
    void define_function(const std::string& name, u16 arity, ScriptObject::callable_type function);
    ScriptObject& find_variable(const Token& name);

private:
    ScriptEnvironment* _enclosing;
    std::unordered_map<std::string, ScriptObject> _variables;
};
