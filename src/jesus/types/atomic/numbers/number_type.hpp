#pragma once

#include "../../creation_type.hpp"

class NumberType : public CreationType
{
public:
    NumberType(std::shared_ptr<CreationType> parent)
        : CreationType(PrimitiveType::Number, "number", "core", parent) {}

    bool validate(const Value &value) const override
    {
        if (value.IS_NUMBER)
            return true;

        throw std::runtime_error("Expected an integer value.");
    }
};
