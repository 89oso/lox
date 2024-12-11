#pragma once

#include "common/common.hpp"

enum ScriptVariableType : u8 { Nil, Boolean, Number, String };

union ScriptVariableValue {
    bool boolean;
    double number;
    const char* string;
};

struct ScriptVariable {
    u8 type;
    ScriptVariableValue value;

    ScriptVariable();
};
