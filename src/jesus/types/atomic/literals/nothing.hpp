#pragma once

#include "../../creation_type.hpp"

/**
 * @brief In Jesus Programming Language, the 'void' type is called 'nothing'.
 *
 * It represents methods that don't return any values.
 * For compatibility, void.hpp exists as an alias.
 *
 * "Now the earth was formless and empty..." — Genesis 1:2
 */
class NothingType : public CreationType
{
public:
    NothingType() : CreationType(PrimitiveType::Null, "nothing", "core") {}

    bool validate(const Value &value) const override
    {
        return value.IS_FORMLESS;
    }
};
