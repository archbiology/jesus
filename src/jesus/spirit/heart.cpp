#include "heart.hpp"
#include <stdexcept>

void Heart::define(const std::string &name, const Value &value)
{
    variables[name] = value;
}

Value Heart::get(const std::string &name) const
{
    auto it = variables.find(name);
    if (it == variables.end())
    {
        throw std::runtime_error("Undefined variable: " + name);
    }

    return it->second;
}

void Heart::assign(const std::string &name, const Value &value)
{
    // auto it = variables.find(name);
    // if (it == variables.end())
    // {
    //     throw std::runtime_error("Cannot assign to undefined variable: " + name);
    // }

    variables[name] = value;
}
