#pragma once

#include "../../creation_type.hpp"

class NaturalType : public CreationType
{
public:
    NaturalType() : CreationType("natural", "core") {}

    bool validate(const Value &value) const override
    {
        return value >= Value(0);
    }
};
