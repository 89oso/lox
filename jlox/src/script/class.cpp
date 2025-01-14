#include "script/class.hpp"
#include "script/class_instance.hpp"

namespace script {

ScriptClass::ScriptClass(const std::string& name)
    : name(name) {
}

ScriptObject ScriptClass::call(Interpreter* interpreter, std::vector<ScriptObject>& arguments) {
    ScriptObject obj;
    obj.type = ScriptObjectType::ClassInstance;

    auto arc_klass = this->shared_from_this();
    obj.value = std::make_shared<ScriptClassInstance>(arc_klass);

    return obj;
}

std::string ScriptClass::to_string() {
    return name;
}

} // namespace script
