#include "script/class_instance.hpp"
#include "script/class.hpp"

namespace script {

ScriptClassInstance::ScriptClassInstance(Arc<ScriptClass>& klass)
    : klass(klass) {
}

std::string ScriptClassInstance::to_string() const {
    return klass->name + " instance";
}

} // namespace script
