#pragma once

#include "../../creation_type.hpp"

class DecimalType : public CreationType
{
public:
    DecimalType() : CreationType("decimal", "core") {}

    bool validate(const Value &value) const override
    {
        return value.IS_NUMBER;
    }
};
