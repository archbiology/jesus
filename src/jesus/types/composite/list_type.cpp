#include "list_type.hpp"
#include "interpreter/runtime/native_method.hpp"
#include "types/known_types.hpp"

Value ListType::addItem(Interpreter &, Value &obj, const std::vector<Value> &args)
{
    obj.asList().push_back(std::make_shared<Value>(args[0])); // FIXME: Shouldn't args be already smart pointers?

    return Value::formless();
}

void ListType::registerMethods()
{
    auto params = std::make_shared<Heart>("list.add");

    params->createVar(elementType, "item", Value::formless());

    addMethod("add", std::make_shared<NativeMethod>("add", params, KnownTypes::VOID, ListType::addItem));
}
