
#include "method.hpp"
#include "../interpreter.hpp"

Value Method::call(Interpreter &interpreter, Value &object, const std::vector<Value> &args)
{
    std::shared_ptr<Instance> instance = object.toInstance();

    // 1. Create the method 'params' scope for this call,
    // passing instance attributes as the parent attributes.
    auto paramsScope = params->clone("call-" + name, instance->attributes);

    // Bind the actual 'arguments' to the 'param' names
    int index = 0;
    for (int index = 0; index < paramsScope->paramsCount; index++)
        paramsScope->updateVar(index, args[index]);

    interpreter.addScope(instance->attributes); // FIXME: should not add two scopes here. SymbolTable::updateVar should instead consider scope->parent_attributes
    interpreter.addScope(paramsScope);

    // The class of this object (this) is the access context used to
    // enforce attribute permissions inside the method body.
    interpreter.pushClassContext(instance->spirit, instance->attributes);

    // 2. Execute method body
    Value returnValue = Value::formless(); // default return

    try {
        for (auto stmt : body)
            interpreter.execute(stmt);
    } catch (const ReturnSignal &ret) {
        returnValue = ret.value;
    }
    catch (...)
    {
        interpreter.popClassContext();
        interpreter.popScope();
        interpreter.popScope();
        throw;
    }

    // 3. Pop the class context, 'params' (and 'attributes') scope
    interpreter.popClassContext();
    interpreter.popScope();
    interpreter.popScope();

    return returnValue;
}
