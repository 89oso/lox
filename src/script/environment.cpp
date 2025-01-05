#include "script/environment.hpp"
#include "common/exception.hpp"

#include <iostream>

ScriptEnvironment::ScriptEnvironment()
    : _enclosing(0) {
}

ScriptEnvironment::ScriptEnvironment(ScriptEnvironment* enclosing)
    : _enclosing(enclosing) {
}

void ScriptEnvironment::set_name(const std::string& name) {
    _name = name;
}

void ScriptEnvironment::assign_variable(const Token& name, ScriptObject& value) {
    auto itr = _variables.find(name.value);
    if (itr != _variables.end()) {
        itr->second.value = value.value;
        return;
    }

    if (_enclosing) {
        _enclosing->assign_variable(name, value);
        return;
    }

    throw RuntimeError(name, "Undefined variable '" + name.value + "'.");
}

void ScriptEnvironment::define_variable(const std::string& name, ScriptObject& value) {
    _variables[name] = value;
}

void ScriptEnvironment::define_function(const std::string& name, u16 arity, ScriptObject::callable_type function) {
    ScriptObject variable;
    variable.type = ScriptObjectType::Callable;
    variable.value = std::make_shared<ScriptObject::Callable>(arity, function);

    _variables.insert({ name, variable });
}

ScriptObject& ScriptEnvironment::find_variable(const Token& name) {
    auto itr = _variables.find(name.value);
    if (itr != _variables.end())
        return itr->second;

    if (_enclosing)
        return _enclosing->find_variable(name);

    throw RuntimeError(name, "Undefined variable '" + name.value + "'.");
}

void ScriptEnvironment::print(u32 indent) {
    std::cout << "-------- ENVIRONMENT DUMP --------\n";
    for (auto& var : _variables) {
        for (u32 i = 0; i < indent; i++)
            std::cout << " ";
        std::cout << var.first << ": type = " << (u32)var.second.type << "\n";
    }

    if (_enclosing) {
        _enclosing->print(indent + 1);
    }

    std::cout << "----------------------------------\n";
}
