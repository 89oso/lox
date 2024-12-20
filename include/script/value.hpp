#pragma once

#include "common/common.hpp"

#include <variant>

enum ScriptValueType : u8 { Nil, Boolean, Number, String };

struct ScriptValue {
    ScriptValue()
        : type(ScriptValueType::Nil) {
    }

    u8 type;
    std::variant<bool, double, std::string> value;
};
