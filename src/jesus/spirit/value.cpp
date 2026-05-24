#include "value.hpp"
#include "../interpreter/runtime/instance.hpp"
#include "../interpreter/runtime/module.hpp"

struct make_string_functor
{
    std::string operator()(const std::string &x) const { return x; }
    std::string operator()(const double x) const { return "(double) " + std::to_string(x); }
    std::string operator()(const int x) const { return "(int) " + std::to_string(x); }
    std::string operator()(const bool x) const { return "(logic) " + std::to_string(x); }
    std::string operator()(const std::monostate x) const { return "null"; }
    std::string operator()(const std::shared_ptr<Module> x) const { return x->toString(); }
    std::string operator()(const std::shared_ptr<CreationType> x) const { return x->toString(); }
    std::string operator()(const std::shared_ptr<Instance> x) const { return x->toString(); }

    std::string operator()(const std::shared_ptr<ListValue> &list) const
    {
        if (!list)
            return "[]";

        std::string result = "[";
        bool first = true;

        for (const auto &item : *list)
        {
            if (!first)
                result += ", ";

            result += item ? item->toString() : "null";
            first = false;
        }

        result += "]";
        return result;
    }

    std::string operator()(const std::shared_ptr<DictValue> &dict) const
    {
        if (!dict)
            return "{}";

        std::string result = "{";
        bool first = true;

        for (const auto &[key, value] : *dict)
        {
            if (!first)
                result += ", ";

            result += (key ? key->toString() : "null");
            result += ": ";
            result += (value ? value->toString() : "null");

            first = false;
        }

        result += "}";

        return result;
    }
};

const std::shared_ptr<Instance> Value::toInstance() const
{
    if (!IS_INSTANCE)
    {
        // FIXME: This has to be validated at parse time
        throw std::runtime_error("Only instances have attributes.");
    }

    return std::get<std::shared_ptr<Instance>>(value);
}

std::string Value::toString() const
{
    return std::visit(make_string_functor(), value);
}
