
#include "method.hpp"
#include "../interpreter.hpp"

Value Method::call(Interpreter &interpreter, std::shared_ptr<Instance> instance, const std::vector<Value> args)
{
    // 1. Add instance attributes to the scope
    interpreter.addScope(instance->attributes);

    // 2. Create and push the 'params' scope (frame) for this call
    auto paramsScope = params->clone("call-" + name);
    interpreter.addScope(paramsScope);

    for (auto stmt : body)
    {
        interpreter.execute(stmt);
    }

    // 3. Pop 'attributes' and 'params' scope
    interpreter.popScope();
    interpreter.popScope();

    return Value::formless(); // default return
}
