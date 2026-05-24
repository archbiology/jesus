#include "list_type.hpp"
#include "interpreter/runtime/native_method.hpp"
#include "types/known_types.hpp"

void ListType::registerMethods()
{
    // -----------
    // add(value)
    // -----------
    {
        auto params = std::make_shared<Heart>("list.add");
        params->createVar(elementType, "item", Value::formless());
        addMethod("add", std::make_shared<NativeMethod>("add", params, KnownTypes::VOID, ListType::addItem));
    }

    // --------------
    // remove(value)
    // --------------
    {
        auto params = std::make_shared<Heart>("list.remove");
        params->createVar(elementType, "item", Value::formless());
        addMethod("remove", std::make_shared<NativeMethod>("remove", params, KnownTypes::BOOLEAN, ListType::removeItem));
    }

    // -----------------
    // remove_at(index)
    // -----------------
    {
        auto params = std::make_shared<Heart>("list.remove_at");
        params->createVar(KnownTypes::INT, "index", Value::formless());
        addMethod("remove_at", std::make_shared<NativeMethod>("remove_at", params, elementType, ListType::removeAt));
    }

    // ---------------
    // remove_first()
    // ---------------
    {
        auto params = std::make_shared<Heart>("list.remove_first");
        addMethod("remove_first", std::make_shared<NativeMethod>("remove_first", params, elementType, ListType::removeFirst));
    }

    // --------------
    // remove_last()
    // --------------
    {
        auto params = std::make_shared<Heart>("list.remove_last");
        addMethod("remove_last", std::make_shared<NativeMethod>("remove_last", params, elementType, ListType::removeLast));
    }

    // ----------------
    // contains(value)
    // ----------------
    {
        auto params = std::make_shared<Heart>("list.contains");
        params->createVar(elementType, "item", Value::formless());
        addMethod("contains", std::make_shared<NativeMethod>("contains", params, KnownTypes::BOOLEAN, ListType::contains));
    }

    // --------
    // count()
    // --------
    {
        auto params = std::make_shared<Heart>("list.count");
        addMethod("count", std::make_shared<NativeMethod>("count", params, KnownTypes::INT, ListType::count));
    }

    // -----------
    // is_empty()
    // -----------
    {
        auto params = std::make_shared<Heart>("list.is_empty");
        addMethod("is_empty", std::make_shared<NativeMethod>("is_empty", params, KnownTypes::BOOLEAN, ListType::isEmpty));
    }

    // --------
    // first()
    // --------
    {
        auto params = std::make_shared<Heart>("list.first");
        addMethod("first", std::make_shared<NativeMethod>("first", params, elementType, ListType::first));
    }

    // -------
    // last()
    // -------
    {
        auto params = std::make_shared<Heart>("list.last");
        addMethod("last", std::make_shared<NativeMethod>("last", params, elementType, ListType::last));
    }

    // --------
    // clear()
    // --------
    {
        auto params = std::make_shared<Heart>("list.clear");
        addMethod("clear", std::make_shared<NativeMethod>("clear", params, KnownTypes::VOID, ListType::clear));
    }

    // ----------
    // reverse()
    // ----------
    {
        auto params = std::make_shared<Heart>("list.reverse");
        addMethod("reverse", std::make_shared<NativeMethod>("reverse", params, KnownTypes::VOID, ListType::reverse));
    }

    // -------
    // sort()
    // -------
    {
        auto params = std::make_shared<Heart>("list.sort");
        addMethod("sort", std::make_shared<NativeMethod>("sort", params, KnownTypes::VOID, ListType::sort));
    }

    // -------
    // copy()
    // -------
    {
        auto params = std::make_shared<Heart>("list.copy");
        addMethod("copy", std::make_shared<NativeMethod>("copy", params, KnownTypes::LIST, ListType::copy));
    }
}

Value ListType::addItem(Interpreter &, Value &obj, const std::vector<Value> &args)
{
    obj.asList().push_back(std::make_shared<Value>(args[0])); // FIXME: Shouldn't args be already smart pointers?

    return Value::formless();
}

Value ListType::removeItem(Interpreter &, Value &obj, const std::vector<Value> &args)
{
    auto &list = obj.asList();

    auto it = std::find_if(
        list.begin(), list.end(), [&](const std::shared_ptr<Value> &v)
        { return *v == args[0]; });

    if (it == list.end())
        return Value(false);

    list.erase(it);
    return Value(true);
}

Value ListType::removeAt(Interpreter &, Value &obj, const std::vector<Value> &args)
{
    auto &list = obj.asList();
    int index = args[0].toInt();

    if (index < 0 || index >= (int)list.size())
        return Value::formless();

    Value removed = *list[index];
    list.erase(list.begin() + index);
    return removed;
}

Value ListType::removeFirst(Interpreter &, Value &obj, const std::vector<Value> &)
{
    auto &list = obj.asList();

    if (list.empty())
        return Value::formless();

    Value removed = *list.front();
    list.erase(list.begin());
    return removed;
}

Value ListType::removeLast(Interpreter &, Value &obj, const std::vector<Value> &)
{
    auto &list = obj.asList();

    if (list.empty())
        return Value::formless();

    Value removed = *list.back();

    list.pop_back();

    return removed;
}

Value ListType::contains(Interpreter &, Value &obj, const std::vector<Value> &args)
{
    auto &list = obj.asList();

    for (const auto &v : list)
    {
        if (*v == args[0])
            return Value(true);
    }

    return Value(false);
}

Value ListType::count(Interpreter &, Value &obj, const std::vector<Value> &)
{
    return Value((int)obj.asList().size());
}

Value ListType::isEmpty(Interpreter &, Value &obj, const std::vector<Value> &)
{
    return Value(obj.asList().empty());
}

Value ListType::first(Interpreter &, Value &obj, const std::vector<Value> &)
{
    auto &list = obj.asList();

    if (list.empty())
        return Value::formless();

    return *list.front();
}

Value ListType::last(Interpreter &, Value &obj, const std::vector<Value> &)
{
    auto &list = obj.asList();

    if (list.empty())
        return Value::formless();

    return *list.back();
}

Value ListType::clear(Interpreter &, Value &obj, const std::vector<Value> &)
{
    obj.asList().clear();

    return Value::formless();
}

Value ListType::reverse(Interpreter &, Value &obj, const std::vector<Value> &)
{
    auto &list = obj.asList();

    std::reverse(list.begin(), list.end());

    return Value::formless();
}

Value ListType::sort(Interpreter &, Value &obj, const std::vector<Value> &)
{
    auto &list = obj.asList();

    if (list.empty())
        return Value::formless();

    auto first = list[0];

    bool sortable = first->IS_NUMBER || first->IS_STRING;

    if (!sortable)
    {
        // FIXME: Validate this at parse time, not runtime.
        throw std::runtime_error("List sort() only supports text and number values.");
    }

    std::sort(
        list.begin(), list.end(),
        [](const std::shared_ptr<Value> &a, const std::shared_ptr<Value> &b)
        { return *a < *b; });

    return Value::formless();
}

static Value deepCopyValue(const Value &value)
{
    if (!value.IS_LIST)
        return value;

    auto result = std::make_shared<ListValue>();

    for (const auto &item : value.asList())
        result->push_back(std::make_shared<Value>(deepCopyValue(*item)));

    return Value(result);
}

Value ListType::copy(Interpreter &, Value &obj, const std::vector<Value> &)
{
    return deepCopyValue(obj);
}
