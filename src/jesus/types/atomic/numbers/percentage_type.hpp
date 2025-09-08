#pragma once

#include "../../creation_type.hpp"

class PercentageType : public CreationType
{
public:
    PercentageType() : CreationType(PrimitiveType::Number, "percentage", "core") {}

    bool validate(const Value &value) const override
    {
        if (Value(0) <= value && value <= Value(100))
            return true;

        throw std::runtime_error("Expected a number between 0 and 100.");
    }
};
