#pragma once

#include "../../creation_type.hpp"

class NegativeType : public CreationType
{
public:
    NegativeType() : CreationType("negative", "core") {}

    bool validate(const Value &value) const override
    {
        return value < Value(0);
    }
};
