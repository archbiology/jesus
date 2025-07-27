#pragma once

#include "constraint.hpp"

class BetweenConstraint : public IConstraint
{
    Value lower;
    Value upper;

public:
    BetweenConstraint(double lower, double upper) : lower(lower), upper(upper) {}

    void validate(const Value &value) const override
    {
        if (lower > value || upper < value)
            throw std::runtime_error("Expected value beween " + lower + " and " + upper + " (inclusive)");
    }
};
