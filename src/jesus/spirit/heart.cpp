#include "heart.hpp"
#include <stdexcept>

uint32_t Heart::nextId = 0;

VariableAddress Heart::declareVar(const VarType &type, const std::string &name, bool isParam)
{
    if (slots.contains(name))
    {
        throw std::runtime_error("The variable '" + name + "' already exists in this scope (" + scope_name + ").");
    }

    uint32_t slot = values.size();

    slots[name] = slot;
    values.emplace_back();         // default-constructed Value
    variableOrder.push_back(name); // preserve insertion order to assign 'args' to method 'params' by index.

    registerVarType(type, name);

    if (isParam)
        paramsCount++;

    return VariableAddress{.slot = slot, .scopeId = scopeId};
}

void Heart::createVar(const VarType &type, const std::string &name, const Value &value, bool isParam)
{
    auto address = declareVar(type, name, isParam);
    updateVar(address, value);
}

Value Heart::getVar(const VariableAddress address) const
{
    if (address.scopeId == scopeId)
        return values[address.slot];

    if (parent_attributes)
    {
        return parent_attributes->getVar(address);
    }

    throw std::runtime_error(
        "Invalid variable address (scopeId=" + std::to_string(address.scopeId) + ", slot=" + std::to_string(address.slot) + ")." +
        " Current scope: " + scope_name + " (scopeId=" + std::to_string(scopeId) + ").");
}

void Heart::updateVar(const uint32_t slot, const Value &value)
{
    if (slot >= values.size())
    {
        throw std::runtime_error("updateVar: Invalid variable slot " + std::to_string(slot) + " (scope: " + scope_name + ", values: " + std::to_string(values.size()) + ")");
    }

    values[slot] = value;
}

void Heart::updateVar(const VariableAddress &address, const Value &value)
{
     // If it exists locally, update here
    if (address.scopeId == scopeId)
    {
        updateVar(address.slot, value);
        return;
    }

    // Otherwise, recurse to parent
    if (parent_attributes)
    {
        parent_attributes->updateVar(address, value);
        return;
    }

    throw std::runtime_error(
        "Cannot assign to undefined variable (scopeId=" + std::to_string(address.scopeId) + ", slot=" + std::to_string(address.slot) + ")." +
        " Current scope: " + scope_name + " (scopeId=" + std::to_string(scopeId) + ").");
}

bool Heart::localVarExists(const std::string &name) const
{
    return slots.contains(name);
}

bool Heart::varExistsInHierarchy(const std::string &name) const
{
    if (localVarExists(name))
        return true;

    if (parent_attributes)
        return parent_attributes->varExistsInHierarchy(name);

    return false;
}
