#pragma once

#include "common/common.hpp"

#include <variant>

enum ScriptObjectType : u8 {
    Nil,
    Boolean,
    Number,
    String,
    Callable,
};

class ScriptCallable;

struct ScriptObject {
    u8 type;

    std::variant<bool, f64, std::string, Arc<ScriptCallable>> value;
};
