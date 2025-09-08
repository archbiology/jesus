#pragma once

#include "../../creation_type.hpp"

class RealType : public CreationType
{
public:
    RealType() : CreationType(PrimitiveType::Number, "real", "core") {}

    bool validate(const Value &value) const override
    {
        if (value.IS_NUMBER)
            return true;

        throw std::runtime_error("Expected a number.");
    }
};
