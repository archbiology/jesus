#pragma once

#include "../../spirit/value.hpp"

class IConstraint
{
public:
    virtual void validate(const Value &value) const = 0;
    virtual ~IConstraint() = default;
};
