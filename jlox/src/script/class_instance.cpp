#include "script/class_instance.hpp"
#include "script/class.hpp"

#include "common/exception.hpp"

namespace script {

ScriptClassInstance::ScriptClassInstance(Arc<ScriptClass>& klass)
    : klass(klass) {
}

ScriptObject& ScriptClassInstance::get(Token& name) {
    if (fields.contains(name.value)) {
        return fields.at(name.value);
    }

    throw RuntimeError(name, "Undefined property '" + name.value + "'");
}

void ScriptClassInstance::set(Token& name, ScriptObject obj) {
    fields[name.value] = obj;
}

std::string ScriptClassInstance::to_string() const {
    return klass->name + " instance";
}

} // namespace script
