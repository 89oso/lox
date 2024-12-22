#include "script/object.hpp"
#include "script/interpreter.hpp"

#include <iostream>

ScriptObject::ScriptObject()
    : type(ScriptObjectType::Nil) {
}

ScriptObject::Callable::Callable()
    : arity(0),
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

ScriptFunction::ScriptFunction(FunctionStmt* decl)
    : decl(decl) {
    arity = decl->params.size();
}

ScriptObject ScriptFunction::call(Interpreter* interpreter, std::vector<ScriptObject>& arguments) {
    ScriptEnvironment environment = interpreter->global_env();

    for (int i = 0; i < decl->params.size(); i++) {
        environment.define_variable(decl->params.at(i).value, arguments.at(i));
    }

    auto boxed_environment = std::make_shared<ScriptEnvironment>(environment);
    interpreter->execute_block(decl->body, boxed_environment);

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
