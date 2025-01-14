#pragma once

#include "common/common.hpp"

#include <variant>

namespace script {

enum ScriptObjectType : u8 {
    Nil,
    Boolean,
    Number,
    String,
    Callable,
    Class,
    ClassInstance,
};

struct ScriptCallable;
struct ScriptClass;
struct ScriptClassInstance;

struct ScriptObject {
    u8 type;

    std::variant<bool, f64, std::string, Arc<ScriptCallable>, Arc<ScriptClass>, Arc<ScriptClassInstance>> value;
};

} // namespace script
