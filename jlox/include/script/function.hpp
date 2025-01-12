#pragma once

#include "script/ast.hpp"
#include "script/callable.hpp"

class ScriptEnvironment;

struct ScriptFunction : ScriptCallable {
    FunctionStmt* decl;
    Arc<ScriptEnvironment> closure;
    bool anonymous;

    ScriptFunction() = delete;
    ScriptFunction(FunctionStmt* decl, Arc<ScriptEnvironment> closure, bool anonymous);

    ScriptObject call(Interpreter* interpreter, std::vector<ScriptObject>& arguments) override;
    std::string to_string() override;
};
