
#include "method.hpp"
#include "../interpreter.hpp"

Value Method::call(Interpreter &interpreter, Instance instance, const std::vector<Value> args)
{

    for (auto stmt : body)
    {
        interpreter.execute(stmt);
    }

    return Value::formless(); // default return
}
