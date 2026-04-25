#pragma once

#include "../creation_type.hpp"

/**
 * @brief Runtime representation of list<T>.
 */
class ListType : public CreationType
{
public:
    std::shared_ptr<CreationType> elementType;

    ListType(std::shared_ptr<CreationType> elementType, std::shared_ptr<CreationType> parent)
        : CreationType(PrimitiveType::Collection, "list", "core", parent),
          elementType(elementType) {}

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
};
