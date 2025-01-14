#pragma once

#include "script/callable.hpp"

namespace script {

struct ScriptClass : ScriptCallable, public std::enable_shared_from_this<ScriptClass>  {
    std::string name;

    ScriptClass() = delete;
    ScriptClass(const std::string& name);

    ScriptObject call(Interpreter* interpreter, std::vector<ScriptObject>& arguments) override;
    std::string to_string() override;
};

} // namespace script
