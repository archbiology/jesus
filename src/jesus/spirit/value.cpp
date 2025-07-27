#include "value.hpp"

struct make_string_functor
{
    std::string operator()(const std::string &x) const { return x; }
    std::string operator()(double x) const { return "(double) " + std::to_string(x); }
    std::string operator()(int x) const { return "(int) " + std::to_string(x); }
    std::string operator()(bool x) const { return "(logic) " + std::to_string(x); }
    std::string operator()(std::monostate x) const { return "null"; }

    std::string operator()(const std::vector<std::shared_ptr<Value>> &list) const
    {
        std::string result = "[";
        bool first = true;

        for (const auto &item : list)
        {
            if (!first)
                result += ", ";

            result += item ? item->toString() : "null";
            first = false;
        }

        result += "]";
        return result;
    }
};

std::string Value::toString() const
{
    return std::visit(make_string_functor(), value);
}
