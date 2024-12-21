#include "script/environment.hpp"
#include "common/exception.hpp"

ScriptEnvironment::ScriptEnvironment()
    : _enclosing(0) {
}

ScriptEnvironment::ScriptEnvironment(ScriptEnvironment* enclosing)
    : _enclosing(enclosing) {
}

void ScriptEnvironment::assign_variable(const Token& name, ScriptValue& value) {
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

void ScriptEnvironment::define_variable(const std::string& name, ScriptValue& value) {
    _variables.insert({ name, value });
}

ScriptValue& ScriptEnvironment::find_variable(const Token& name) {
    auto itr = _variables.find(name.value);
    if (itr != _variables.end())
        return itr->second;

    if (_enclosing)
        return _enclosing->find_variable(name);

    throw RuntimeError(name, "Undefined variable '" + name.value + "'.");
}
