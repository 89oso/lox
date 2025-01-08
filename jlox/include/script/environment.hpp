#pragma once

#include "token.hpp"
#include "object.hpp"

#include <unordered_map>

class ScriptEnvironment {
public:
    ScriptEnvironment();
    ScriptEnvironment(ScriptEnvironment* enclosing);

    void set_name(const std::string& name);

    void assign_variable(const Token& name, ScriptObject& value);
    void assign_variable_at(usize distance, const Token& name, ScriptObject& value);

    void define_variable(const std::string& name, ScriptObject& value);
    void define_function(const std::string& name, u16 arity, ScriptObject::callable_type function);

    ScriptObject& find_variable(const Token& name);
    ScriptObject& find_variable_at(usize distance, const Token& name);

    void print(u32 indent);

private:
    std::string _name;
    ScriptEnvironment* _enclosing;
    std::unordered_map<std::string, ScriptObject> _variables;

    ScriptEnvironment* get_ancestor(usize distance);
};
