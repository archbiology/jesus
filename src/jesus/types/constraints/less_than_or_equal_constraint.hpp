#pragma once

#include "constraint.hpp"

class LessThanOrEqualConstraint : public IConstraint
{
    Value threshold;

public:
    LessThanOrEqualConstraint(double threshold) : threshold(threshold) {}

    void validate(const Value &value) const override
    {
        if (!value.IS_NUMBER || value > threshold)
            throw std::runtime_error("Expected value <= " + threshold);
    }
};
