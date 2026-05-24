#pragma once

#include "types/creation_type.hpp"

/**
 * @brief Runtime representation of dict<Key,Value>.
 */
class DictType : public CreationType
{
public:
    std::shared_ptr<CreationType> keyType;
    std::shared_ptr<CreationType> valueType;

    DictType(
        std::shared_ptr<CreationType> keyType,
        std::shared_ptr<CreationType> valueType,
        std::shared_ptr<CreationType> parent,
        std::string name = "dict")
        : CreationType(PrimitiveType::Collection, name, "core", parent),
          keyType(keyType), valueType(valueType)
    {
        registerMethods();
    }

    bool validate(const Value &value) const override
    {
        if (!value.IS_DICT)
            return false;

        for (const auto &[k, v] : value.asDict())
        {
            if (!keyType->validate(*k))
                return false;

            if (!valueType->validate(*v))
                return false;
        }

        return true;
    }

private:
    void registerMethods();
};
