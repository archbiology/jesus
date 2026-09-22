#include "value.hpp"
#include "interpreter/runtime/instance.hpp"
#include "interpreter/runtime/module.hpp"
#include "interpreter/runtime/method.hpp"

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
    std::string operator()(const std::shared_ptr<IMethod> x) const { return x->toString(); }

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

bool operator==(const Value &left, const Value &right)
{
    // --------------------------------------------------------------
    // Numbers, booleans, strings and null compare in the natural way
    // --------------------------------------------------------------
    if (left.IS_FORMLESS && right.IS_FORMLESS)
        return true;

    if (left.IS_NUMBER && right.IS_NUMBER)
    {
        double leftN = left.toNumber();
        double rightN = right.toNumber();

        return leftN == rightN;
    }

    if (left.IS_BOOLEAN && right.IS_BOOLEAN)
        return std::get<bool>(left.value) == std::get<bool>(right.value);

    if (left.IS_STRING && right.IS_STRING)
        return std::get<std::string>(left.value) == std::get<std::string>(right.value);

    if (left.IS_INSTANCE && right.IS_INSTANCE)
    {
        std::shared_ptr<Instance> leftInstance = left.toInstance();
        std::shared_ptr<Instance> rightInstance = right.toInstance();

        // Two nulls compare equal; a null never equals a real instance.
        if (!leftInstance || !rightInstance)
            return leftInstance.get() == rightInstance.get();

        // -------------------------------------------------------------------
        // ENUM MEMBER EQUALITY:
        //  (a) they belong to the SAME enum type, and
        //  (b) they share the SAME MEMBER NAME.
        //
        // We compare the NAME rather than the numeric value because enum values
        // need not be unique (duplicates are legal): `Result Success` and
        // `Result AlsoSuccess`, both carrying value 1, must remain distinct.
        // -------------------------------------------------------------------
        bool sameEnumType = leftInstance->spirit && rightInstance->spirit && leftInstance->spirit->isEnum() &&
                            rightInstance->spirit->isEnum() && leftInstance->spirit->id == rightInstance->spirit->id;

        if (sameEnumType)
            return leftInstance->toString() == rightInstance->toString();

        return false;
    }

    return false; // values of different types are never equal
}

const std::shared_ptr<IMethod> Value::asMethod() const
{
    if (!IS_METHOD)
    {
        // FIXME: This has to be validated at parse time
        throw std::runtime_error("Only methods can be returned as methods.");
    }

    return std::get<std::shared_ptr<IMethod>>(value);
}

std::string Value::toString() const
{
    return std::visit(make_string_functor(), value);
}
