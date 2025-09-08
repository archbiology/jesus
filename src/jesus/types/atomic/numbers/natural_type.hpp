#pragma once

#include "../../creation_type.hpp"
#include "../../constraints/greater_than_or_equal_constraint.hpp"

class NaturalType : public CreationType
{
public:
    NaturalType() : CreationType(PrimitiveType::Number, "natural", "core") {}

    bool validate(const Value &value) const override
    {
        GreaterThanOrEqualConstraint(0).validate(value);

        return true;
    }
};
