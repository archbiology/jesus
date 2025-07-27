#pragma once

#include "../../creation_type.hpp"

class PercentageType : public CreationType
{
public:
    PercentageType() : CreationType(PrimitiveType::Number, "percentage", "core") {}

    bool validate(const Value &value) const override
    {
        return Value(0) <= value && value <= Value(100);
    }
};
