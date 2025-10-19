#pragma once

#include "../../creation_type.hpp"

class RealType : public CreationType
{
public:
    RealType(std::shared_ptr<CreationType> parent)
        : CreationType(PrimitiveType::Number, "real", "core", parent) {}

    bool validate(const Value &value) const override
    {
        if (value.IS_NUMBER)
            return true;

        throw std::runtime_error("Expected a number.");
    }
};
