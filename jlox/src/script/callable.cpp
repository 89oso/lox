#include "script/callable.hpp"

ScriptCallable::ScriptCallable()
    : type(ScriptCallableType::Invalid),
      arity(0),
      function(0) {
}

ScriptCallable::ScriptCallable(u16 arity, function_type& function)
    : arity(arity),
      function(function) {
}

ScriptObject ScriptCallable::call(Interpreter* interpreter, std::vector<ScriptObject>& arguments) {
    return function(interpreter, arguments);
}

std::string ScriptCallable::to_string() {
    return "<fn>";
}
