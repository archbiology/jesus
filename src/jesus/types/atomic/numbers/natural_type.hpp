#pragma once

#include "../../creation_type.hpp"
#include "../../constraints/greater_than_or_equal_constraint.hpp"

class NaturalType : public CreationType
{
public:
    NaturalType(std::shared_ptr<CreationType> parent)
        : CreationType(PrimitiveType::Number, "natural", "core", parent) {}

    bool validate(const Value &value) const override
    {
        GreaterThanOrEqualConstraint(0).validate(value);

        return true;
    }
};
