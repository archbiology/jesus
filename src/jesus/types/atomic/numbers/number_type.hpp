#pragma once

#include "../../creation_type.hpp"

class NumberType : public CreationType
{
public:
    NumberType() : CreationType("number", "core") {}

    bool validate(const Value &value) const override
    {
        return value.IS_INT;
    }
};
