#include "heart.hpp"
#include <stdexcept>

void Heart::createVar(const std::string &type, const std::string &name, const Value &value)
{
    if (localVarExists(name))
    {
        throw std::runtime_error("The variable '" + name + "' already exists in this scope (" + scope_name + ").");
    }

    variables[name] = value;
    registerVarType(type, name);

    variableOrder.push_back(name); // preserve insertion order to assign 'args' to method 'params' by index.
}

Value Heart::getVar(const std::string &name) const
{
    auto it = variables.find(name);
    if (it != variables.end())
    {
        return it->second;
    }

    if (parent_attributes)
    {
        return parent_attributes->getVar(name);
    }

    throw std::runtime_error("Undefined variable: " + name + " (scope: " + scope_name + ")");
}

void Heart::updateVar(const std::string &name, const Value &value)
{
     // If it exists locally, update here
    auto it = variables.find(name);
    if (it != variables.end())
    {
        it->second = value;
        return;
    }

    // Otherwise, recurse to parent
    if (parent_attributes)
    {
        parent_attributes->updateVar(name, value);
        return;
    }

    throw std::runtime_error("Cannot assign to undefined variable: " + name + " (scope: " + scope_name + ")");
}

bool Heart::localVarExists(const std::string &name) const
{
    return variables.find(name) != variables.end();
}

bool Heart::varExistsInHierarchy(const std::string &name) const
{
    if (localVarExists(name))
        return true;

    if (parent_attributes)
        return parent_attributes->varExistsInHierarchy(name);

    return false;
}
