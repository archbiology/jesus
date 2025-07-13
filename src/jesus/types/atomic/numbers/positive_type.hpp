#pragma once

#include "../../creation_type.hpp"

class PositiveType : public CreationType
{
public:
    PositiveType() : CreationType("positive", "core") {}

    bool validate(const Value &value) const override
    {
        return value > Value(0);
    }
};
