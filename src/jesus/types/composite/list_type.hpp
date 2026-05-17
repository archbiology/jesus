#pragma once

#include "../creation_type.hpp"

class Interpreter; // Forward declaration

/**
 * @brief Runtime representation of list<T>.
 */
class ListType : public CreationType
{
public:
    std::shared_ptr<CreationType> elementType;

    ListType(std::shared_ptr<CreationType> elementType, std::shared_ptr<CreationType> parent, std::string name = "list")
        : CreationType(PrimitiveType::Collection, name, "core", parent),
          elementType(elementType)
    {
        registerMethods();
    }

    bool validate(const Value &value) const override
    {
        if (!value.IS_LIST)
            return false;

        for (const auto &v : value.asList())
        {
            if (!elementType->validate(*v))  // FIXME: Isn't this '*' slow?
                return false;
        }

        return true;
    }

private:
    void registerMethods();

    static Value addItem(Interpreter &interp, Value &obj, const std::vector<Value> &args);
};
