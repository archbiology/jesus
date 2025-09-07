#pragma once

#include "../../creation_type.hpp"

/**
 * @brief Literal values: male, female
 *
 * "So God created mankind in his own image,
 * in the image of God he created them;
 * male and female he created them." — Genesis 1:27
 */
class SexType : public CreationType
{
public:
    SexType() : CreationType(PrimitiveType::Boolean, "sex", "core") {}

    bool validate(const Value &value) const override
    {
        return value.IS_BOOLEAN;
    }
};
