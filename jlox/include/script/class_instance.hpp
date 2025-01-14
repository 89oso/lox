#pragma once

#include "script/object.hpp"
#include "script/token.hpp"

#include <unordered_map>

namespace script {

struct ScriptClass;

struct ScriptClassInstance {
    Arc<ScriptClass> klass;
    std::unordered_map<std::string, ScriptObject> fields;

    ScriptClassInstance() = delete;
    ScriptClassInstance(Arc<ScriptClass>& klass);

    ScriptObject& get(Token& name);
    void set(Token& name, ScriptObject value);

    std::string to_string() const;
};

} // namespace script
