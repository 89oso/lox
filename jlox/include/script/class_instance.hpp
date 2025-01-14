#pragma once

#include "common/common.hpp"

namespace script {

struct ScriptClass;

struct ScriptClassInstance {
    Arc<ScriptClass> klass;

    ScriptClassInstance() = delete;
    ScriptClassInstance(Arc<ScriptClass>& klass);

    std::string to_string() const;
};

} // namespace script
