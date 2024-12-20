#include "script/environment.hpp"
#include "common/exception.hpp"

ScriptEnvironment::ScriptEnvironment() {
}

void ScriptEnvironment::assign_variable(const Token& name, ScriptValue& value) {
    auto itr = _variables.find(name.value);
    if (itr == _variables.end())
        throw RuntimeError(name, "Undefined variable '" + name.value + "'.");

    itr->second.value = value.value;
}

void ScriptEnvironment::define_variable(const std::string& name, ScriptValue& value) {
    _variables.insert({ name, value });
}

ScriptValue& ScriptEnvironment::find_variable(const Token& name) {
    auto itr = _variables.find(name.value);
    if (itr == _variables.end())
        throw RuntimeError(name, "Undefined variable '" + name.value + "'.");

    return itr->second;
}
