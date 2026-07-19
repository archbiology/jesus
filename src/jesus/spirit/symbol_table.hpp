#pragma once

#include <memory>
#include "heart.hpp"

REGISTER_FOR_UML(
    SymbolTable,
    .fieldsList({"scopes: Heart"})
        .methodsList({"addScope", "popScope",
                      "createVar", "getVar", "updateVar",
                      "registerVarType", "updatePolymorphicVarType",
                      "registerClassName"}));

class SymbolTable
{
    std::vector<std::shared_ptr<Heart>> scopes;
    std::shared_ptr<Heart> current_scope;

public:
    explicit SymbolTable(std::shared_ptr<Heart> global_scope)
    {
        // Push the main scope, the Global Symbol Table
        addScope(global_scope);
    }

    std::shared_ptr<Heart> currentScope()
    {
        return current_scope;
    }

    void addScope(std::shared_ptr<Heart> scope)
    {
        scopes.emplace_back(scope);
        current_scope = scope;
    }

    void popScope()
    {

        if (!scopes.empty())
        {
            scopes.pop_back();

            if (scopes.empty())
                throw std::runtime_error("SymbolTable: Has the Global Scope been removed?");

            current_scope = scopes.back();
        }
    }

    void createVar(const VarType &type, const std::string &name, const Value &value, bool isParam)
    {
        current_scope->createVar(type, name, value, isParam);
    }

    VariableAddress resolveVariableAddress(const std::string &name)
    {
        for (auto scope = scopes.rbegin(); scope != scopes.rend(); ++scope)
        {
            if ((*scope)->varExistsInHierarchy(name))
                return (*scope)->resolveVariableAddressInHierarchy(name);
        }

        throw std::runtime_error("Undefined variable: " + name + " (scope: " + current_scope->scope_name + ")");
    }

    Value getVar(const std::string &name) const
    {
        // FIXME: Use only 'getVar(const VariableAddress address)', which is faster then this 'getVar(const std::string &name)'
        for (auto scope = scopes.rbegin(); scope != scopes.rend(); ++scope)
        {
            if ((*scope)->varExistsInHierarchy(name))
            {
                auto address = (*scope)->resolveVariableAddressInHierarchy(name);
                return (*scope)->getVar(address);
            }
        }

        throw std::runtime_error("Undefined variable: " + name + " (scope: " + current_scope->scope_name + ")");
    }

    Value getVar(const VariableAddress address) const
    {
        // Iterate over the scopes in reverse order (rbegin, rend)
        for (auto scope = scopes.rbegin(); scope != scopes.rend(); ++scope)
        {
            if ((*scope)->scopeId == address.scopeId)
                return (*scope)->getVar(address);
        }

        return current_scope->getVar(address);
    }

    void updateVar(const std::string &name, const Value &value)
    {
        // FIXME: Use only 'updateVar(const VariableAddress address', which is faster then this 'updateVar(const std::string &name'
        for (auto scope = scopes.rbegin(); scope != scopes.rend(); ++scope)
        {
            if ((*scope)->varExistsInHierarchy(name))
            {
                auto address = (*scope)->resolveVariableAddressInHierarchy(name);
                (*scope)->updateVar(address, value);
                return;
            }
        }

        throw std::runtime_error("Undefined variable: " + name + " (scope: " + current_scope->scope_name + ")");
    }

    void updateVar(const VariableAddress address, const Value &value)
    {
        // Iterate over the scopes in reverse order (rbegin, rend)
        for (auto scope = scopes.rbegin(); scope != scopes.rend(); ++scope)
        {
            if ((*scope)->scopeId == address.scopeId)
            {
                (*scope)->updateVar(address.slot, value);

                return;
            }
        }

        throw std::runtime_error("Invalid variable address (scopeId=" + std::to_string(address.scopeId) + ", slot=" + std::to_string(address.slot) + ").");
    }

    std::shared_ptr<CreationType> getVarType(const std::string &varName)
    {
        // Iterate over the scopes in reverse order (rbegin, rend)
        for (auto it = scopes.rbegin(); it != scopes.rend(); ++it)
        {
            auto type = (*it)->getVarType(varName);
            if (type != nullptr)
                return type;
        }

        throw std::runtime_error("Variable '" + varName + "' not found. Are you really sure it has been declared?");
    }

    void registerVarType(const VarType &type, const std::string &name)
    {
        current_scope->registerVarType(type, name);
    }

    VariableAddress declareVar(const VarType &type, const std::string &name)
    {
        const bool isParam = false;
        return current_scope->declareVar(type, name, isParam);
    }

    void updatePolymorphicVarType(const std::string &name, const VarType &type)
    {
        current_scope->updatePolymorphicVarType(name, type);
    }

    void registerClassName(const std::string &className)
    {
        current_scope->registerClassName(className);
    }

    bool isClassKnown(const std::string &className) const
    {
        for (auto it = scopes.rbegin(); it != scopes.rend(); ++it)
        {
            if ((*it)->isClassKnown(className))
                return true;
        }
        return false;
    }

    bool varExistsInHierarchy(const std::string &name)
    {
        return current_scope->varExistsInHierarchy(name);
    }
};
