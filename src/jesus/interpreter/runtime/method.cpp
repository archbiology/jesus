
#include "method.hpp"
#include "ast/stmt/create_method_stmt.hpp"
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

    // Bind the hidden '$self' variable ('my'/'I') to the object this method
    // is running on. Any user method runs inside a class body and has the
    // hidden variable.
    if (paramsScope->localVarExists(SELF_VARIABLE))
        paramsScope->updateVar(paramsScope->resolveVariableAddressInHierarchy(SELF_VARIABLE), Value(instance));

    interpreter.addScope(instance->attributes); // FIXME: should not add two scopes here. SymbolTable::updateVar should instead consider scope->parent_attributes
    interpreter.addScope(paramsScope);

    // The class of this object (this) is the access context used to
    // enforce attribute permissions inside the method body.
    interpreter.pushClassContext(instance->spirit, instance->attributes);

    // 2. Execute method body
    Value returnValue = Value::formless(); // default return

    try {
        for (const auto& stmt : definition->body)
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
