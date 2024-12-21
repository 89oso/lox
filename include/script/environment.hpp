#pragma once

#include "token.hpp"
#include "value.hpp"

#include <unordered_map>

class ScriptEnvironment {
public:
    ScriptEnvironment();
    ScriptEnvironment(ScriptEnvironment* enclosing);

    void assign_variable(const Token& name, ScriptValue& value);
    void define_variable(const std::string& name, ScriptValue& value);
    ScriptValue& find_variable(const Token& name);

private:
    ScriptEnvironment* _enclosing;
    std::unordered_map<std::string, ScriptValue> _variables;
};
