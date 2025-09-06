
#include "method.hpp"
#include "../interpreter.hpp"

Value Method::call(Interpreter &interpreter, std::shared_ptr<Instance> instance, const std::vector<Value> args)
{

    // 1. Create a new runtime scope (frame) for this call
    auto paramsScope = params->clone("call-" + name);

    // 2. Push scope
    interpreter.addScope(paramsScope);

    for (auto stmt : body)
    {
        interpreter.execute(stmt);
    }

    // 3. Pop scope
    interpreter.popScope();

    return Value::formless(); // default return
}
