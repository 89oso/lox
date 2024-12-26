#include "script/object.hpp"
#include "script/interpreter.hpp"

#include <iostream>

ScriptObject::ScriptObject()
    : type(ScriptObjectType::Nil) {
}

ScriptObject::Callable::Callable()
    : arity(0),
      type(ScriptCallableType::Builtin),
      function(0) {
}

ScriptObject::Callable::Callable(u16 arity, callable_type function)
    : arity(0),
      function(function) {
}

ScriptObject ScriptObject::Callable::call(Interpreter* interpreter, std::vector<ScriptObject>& arguments) {
    return function(interpreter, arguments);
}

std::string ScriptObject::Callable::to_string() {
    return "<builtin_fn>";
}

ScriptFunction::ScriptFunction(FunctionStmt* decl, std::shared_ptr<ScriptEnvironment> closure, bool anonymous)
    : decl(decl),
      closure(closure),
      anonymous(anonymous) {
    arity = decl->params.size();
    type = ScriptCallableType::Function;
}

ScriptObject ScriptFunction::call(Interpreter* interpreter, std::vector<ScriptObject>& arguments) {
    std::shared_ptr<ScriptEnvironment> environment = closure;

    for (int i = 0; i < decl->params.size(); i++) {
        auto& arg = arguments.at(i);
        auto& param = decl->params.at(i);

        // rename anonymous function to param name if possible
        if (arg.type == ScriptObjectType::Callable) {
            auto callable = std::get<ScriptObject::Callable::ptr>(arg.value);
            if (callable->type == ScriptCallableType::Function) {
                auto* func = reinterpret_cast<ScriptFunction*>(callable.get());
                if (func->anonymous) {
                    func->decl->name.value = param.value;
                }
            }
        }

        environment->define_variable(param.value, arg);
    }

    interpreter->execute_block(decl->body, environment);

    if (interpreter->control_flow_state() == ControlFlowState::Return) {
        interpreter->set_control_flow_state(ControlFlowState::None);
        return interpreter->expr_result();
    }

    ScriptObject result;
    result.type = ScriptObjectType::Nil;
    return result;
}

std::string ScriptFunction::to_string() {
    return "<fn " + decl->name.value + ">";
}
