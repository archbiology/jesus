#pragma once

#include "../../creation_type.hpp"

class RealType : public CreationType
{
public:
    RealType() : CreationType("real", "core") {}

    bool validate(const Value &value) const override
    {
        return value.IS_NUMBER;
    }
};
