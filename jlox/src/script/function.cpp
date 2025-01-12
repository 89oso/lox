#include "script/function.hpp"
#include "script/interpreter.hpp"

ScriptFunction::ScriptFunction(FunctionStmt* decl, Arc<ScriptEnvironment> closure, bool anonymous)
    : decl(decl),
      closure(closure),
      anonymous(anonymous) {
    type = ScriptCallableType::Function;
    arity = decl->params.size();
}

ScriptObject ScriptFunction::call(Interpreter* interpreter, std::vector<ScriptObject>& arguments) {
    Arc<ScriptEnvironment> environment = closure;

    for (i32 i = 0; i < decl->params.size(); i++) {
        auto& arg = arguments.at(i);
        auto& param = decl->params.at(i);

        // rename anonymous function to param name if possible
        if (arg.type == ScriptObjectType::Callable) {
            auto callable = std::get<Arc<ScriptCallable>>(arg.value);
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
    return "<fn>";
}
