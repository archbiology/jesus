#include "dict_type.hpp"
#include "interpreter/interpreter.hpp"
#include "interpreter/runtime/native_method.hpp"
#include "types/known_types.hpp"

void DictType::registerMethods()
{
    // ----------------
    // contains(key)
    // ----------------
    {
        auto params = std::make_shared<Heart>("dict.contains");
        params->createVar(keyType, "key", Value::formless());
        addMethod("contains", std::make_shared<NativeMethod>("contains", params, KnownTypes::BOOLEAN, DictType::contains));
    }

    // -------------
    // remove(key)
    // -------------
    {
        auto params = std::make_shared<Heart>("dict.remove");
        params->createVar(keyType, "key", Value::formless());
        addMethod("remove", std::make_shared<NativeMethod>("remove", params, KnownTypes::BOOLEAN, DictType::remove));
    }

    // --------
    // count()
    // --------
    {
        auto params = std::make_shared<Heart>("dict.count");
        addMethod("count", std::make_shared<NativeMethod>("count", params, KnownTypes::INT, DictType::count));
    }

    // -----------
    // is_empty()
    // -----------
    {
        auto params = std::make_shared<Heart>("dict.is_empty");
        addMethod("is_empty", std::make_shared<NativeMethod>("is_empty", params, KnownTypes::BOOLEAN, DictType::isEmpty));
    }

    // --------
    // clear()
    // --------
    {
        auto params = std::make_shared<Heart>("dict.clear");
        addMethod("clear", std::make_shared<NativeMethod>("clear", params, KnownTypes::VOID, DictType::clear));
    }

    // -------
    // keys()
    // -------
    {
        auto params = std::make_shared<Heart>("dict.keys");
        addMethod("keys", std::make_shared<NativeMethod>("keys", params, KnownTypes::LIST, DictType::keys));
    }

    // ---------
    // values()
    // ---------
    {
        auto params = std::make_shared<Heart>("dict.values");
        addMethod("values", std::make_shared<NativeMethod>("values", params, KnownTypes::LIST, DictType::values));
    }

    // --------
    // pairs()
    // --------
    {
        auto params = std::make_shared<Heart>("dict.pairs");
        addMethod("pairs", std::make_shared<NativeMethod>("pairs", params, KnownTypes::LIST, DictType::pairs));
    }

    // ------
    // get(key, ?default)
    // ------
    {
        auto params = std::make_shared<Heart>("dict.get");
        params->createVar(keyType, "key", Value::formless());
        params->createVar(valueType, "default", Value::formless());
        addMethod("get", std::make_shared<NativeMethod>("get", params, valueType, DictType::get));
    }

    // -------
    // copy()
    // -------
    {
        auto params = std::make_shared<Heart>("dict.copy");
        addMethod("copy", std::make_shared<NativeMethod>("copy", params, KnownTypes::DICT, DictType::copy));
    }

    // ---------
    // update(other)
    // ---------
    {
        auto params = std::make_shared<Heart>("dict.update");
        params->createVar(KnownTypes::DICT, "other", Value::formless());
        addMethod("update", std::make_shared<NativeMethod>("update", params, KnownTypes::VOID, DictType::update));
    }
}

Value DictType::contains(Interpreter &, Value &obj, const std::vector<Value> &args)
{
    auto &dict = obj.asDict();

    for (const auto &[k, v] : dict)
    {
        if (*k == args[0])
            return Value(true);
    }

    return Value(false);
}

Value DictType::remove(Interpreter &, Value &obj, const std::vector<Value> &args)
{
    auto &dict = obj.asDict();

    for (auto it = dict.begin(); it != dict.end(); ++it)
    {
        if (*it->first == args[0])
        {
            dict.erase(it);
            return Value(true);
        }
    }

    return Value(false);
}

Value DictType::count(Interpreter &, Value &obj, const std::vector<Value> &)
{
    return Value((int)obj.asDict().size());
}

Value DictType::isEmpty(Interpreter &, Value &obj, const std::vector<Value> &)
{
    return Value(obj.asDict().empty());
}

Value DictType::clear(Interpreter &, Value &obj, const std::vector<Value> &)
{
    obj.asDict().clear();

    return Value::formless();
}

Value DictType::keys(Interpreter &, Value &obj, const std::vector<Value> &)
{
    auto result = std::make_shared<ListValue>();

    for (const auto &[k, v] : obj.asDict())
        result->push_back(std::make_shared<Value>(*k));

    return Value(result);
}

Value DictType::values(Interpreter &, Value &obj, const std::vector<Value> &)
{
    auto result = std::make_shared<ListValue>();

    for (const auto &[k, v] : obj.asDict())
        result->push_back(std::make_shared<Value>(*v));

    return Value(result);
}

Value DictType::pairs(Interpreter &, Value &obj, const std::vector<Value> &)
{
    auto result = std::make_shared<ListValue>();

    for (const auto &[k, v] : obj.asDict())
    {
        auto pairList = std::make_shared<ListValue>();

        pairList->push_back(std::make_shared<Value>(*k));
        pairList->push_back(std::make_shared<Value>(*v));

        result->push_back(std::make_shared<Value>(pairList));
    }

    return Value(result);
}

Value DictType::get(Interpreter &, Value &obj, const std::vector<Value> &args)
{
    for (const auto &[k, v] : obj.asDict())
    {
        if (*k == args[0])
            return *v;
    }

    return args[1];
}

static Value deepCopyValue(const Value &value)
{
    // ----------
    // LIST
    // ----------
    if (value.IS_LIST)
    {
        auto result = std::make_shared<ListValue>();

        for (const auto &item : value.asList())
            result->push_back(std::make_shared<Value>(deepCopyValue(*item)));

        return Value(result);
    }

    // -----
    // DICT
    // -----
    if (value.IS_DICT)
    {
        auto result = std::make_shared<DictValue>();

        for (const auto &[k, v] : value.asDict())
        {
            result->push_back({std::make_shared<Value>(deepCopyValue(*k)),
                            std::make_shared<Value>(deepCopyValue(*v))});
        }

        return Value(result);
    }

    return value;
}

Value DictType::copy(Interpreter &, Value &obj, const std::vector<Value> &)
{
    return deepCopyValue(obj);
}

Value DictType::update(Interpreter &, Value &obj, const std::vector<Value> &args)
{
    auto &dict = obj.asDict();
    auto &other = args[0].asDict();

    for (const auto &[k, v] : other)
    {
        bool replaced = false;

        for (auto &[existingKey, existingValue] : dict)
        {
            if (*existingKey == *k)
            {
                existingValue = std::make_shared<Value>(*v);
                replaced = true;
                break;
            }
        }

        if (!replaced)
        {
            dict.push_back({std::make_shared<Value>(*k), std::make_shared<Value>(*v)});
        }
    }

    return Value::formless();
}
