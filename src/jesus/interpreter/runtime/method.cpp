
#include "method.hpp"
#include "../interpreter.hpp"

Value Method::call(Interpreter &interpreter, std::shared_ptr<Instance> instance, const std::vector<Value> args)
{
    // 1. Add instance 'attributes' to the scope
    interpreter.addScope(instance->attributes);

    // 2. Create the method 'params' scope for this call
    auto paramsScope = params->clone("call-" + name, instance->attributes); 

    // Bind the actual 'arguments' to the 'param' names
    int index = 0;
    for (const auto &name : paramsScope->getVariableNames())
    {
        if (index < args.size())
            paramsScope->updateVar(name, args[index++]);
        else
            // FIXME: speed it up: Validate params at parse time, not at rutime
            throw std::runtime_error("Missing argument for parameter '" + name + "'");
    }

    interpreter.addScope(paramsScope);

    // 3. Execute method body
    Value returnValue = Value::formless(); // default return

    try {
        for (auto stmt : body)
            interpreter.execute(stmt);
    } catch (const ReturnSignal &ret) {
        returnValue = ret.value;
    }

    // 4. Pop 'attributes' and 'params' scope
    interpreter.popScope();
    interpreter.popScope();

    return returnValue;
}
