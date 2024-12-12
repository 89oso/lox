#pragma once

#include "common/common.hpp"

enum ScriptVariableType : u8 { Nil, Boolean, Number, String };

struct ScriptVariable {
    ScriptVariable();

    ScriptVariable(const ScriptVariable& src) {
        *this = src;
    }

    ScriptVariable& operator=(const ScriptVariable& rhs) {
        type = rhs.type;

        if (type == ScriptVariableType::String)
            value.string = rhs.value.string;
        else
            value.number = rhs.value.number;

        return *this;
    }

    u8 type;

    union Value {
        bool boolean;
        double number;
        std::string string;

        Value() {
        }

        ~Value() {
        }
    } value;
};
