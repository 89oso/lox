#include "script/environment.hpp"
#include "common/exception.hpp"

#include <iostream>

namespace script {

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

void ScriptEnvironment::assign_variable_at(usize distance, const Token& name, ScriptObject& value) {
    ScriptEnvironment* ancestor = get_ancestor(distance);

    // TODO: ensure that we can do obj == obj
    ancestor->_variables[name.value] = value;
}

void ScriptEnvironment::define_variable(const std::string& name, ScriptObject& value) {
    _variables[name] = value;
}

void ScriptEnvironment::define_function(const std::string& name, u16 arity, ScriptCallable::function_type& function) {
    ScriptObject variable;
    variable.type = ScriptObjectType::Callable;
    variable.value = std::make_shared<ScriptCallable>(arity, function); // TODO: Arc::make<ScriptCallable>()

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

ScriptObject& ScriptEnvironment::find_variable_at(usize distance, const Token& name) {
    ScriptEnvironment* ancestor = get_ancestor(distance);
    return ancestor->_variables[name.value];
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

ScriptEnvironment* ScriptEnvironment::get_ancestor(usize distance) {
    ScriptEnvironment* ancestor = this;

    for (usize i = 0; i < distance; i++) {
        ancestor = ancestor->_enclosing;
    }

    return ancestor;
}

} // namespace script
