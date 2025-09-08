#pragma once

#include "../../creation_type.hpp"

class DecimalType : public CreationType
{
public:
    DecimalType() : CreationType(PrimitiveType::Number, "decimal", "core") {}

    bool validate(const Value &value) const override
    {
        if (value.IS_NUMBER)
            return true;

        throw std::runtime_error("Expected a number.");
    }
};
