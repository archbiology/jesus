#include "heart.hpp"
#include <stdexcept>

void Heart::createVar(const std::string &name, const Value &value)
{
    if (varExists(name))
    {
        throw std::runtime_error("The variable '" + name + "' already exists.");
    }

    variables[name] = value;
}

Value Heart::getVar(const std::string &name) const
{
    auto it = variables.find(name);
    if (it == variables.end())
    {
        throw std::runtime_error("Undefined variable: " + name);
    }

    return it->second;
}

void Heart::updateVar(const std::string &name, const Value &value)
{
    if (!varExists(name))
    {
        throw std::runtime_error("Cannot assign to undefined variable: " + name);
    }

    variables[name] = value;
}

bool Heart::varExists(const std::string &name) const
{
    return variables.find(name) != variables.end();
}
