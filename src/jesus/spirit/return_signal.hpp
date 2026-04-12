#pragma once
#include "value.hpp"

/**
 * @brief Returned when we have 'return' inside a function.
 */
class ReturnSignal
{
public:
    Value value;

public:
    ReturnSignal(Value value) : value(std::move(value)) {}
};
