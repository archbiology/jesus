#pragma once

#include <memory>
#include "heart.hpp"

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

    void createVar(const std::string &type, const std::string &name, const Value &value)
    {
        current_scope->createVar(type, name, value);
    }

    Value getVar(const std::string &name) const
    {
        // FIXME: speed it up:
        //  1 - Store scope level 'enum class ScopeLevel { PARAMS, INSTANCE_ATTR, GLOBAL };' at parseTime on GetParamExpr|GetAttributeExpr|GetGlobalVarExpr
        //  2 - store an index into the Heart (like int slot) at parse time, so getVar doesn’t even need to hash the 'name' at runtime — just array access.

        // Iterate over the scopes in reverse order (rbegin, rend)
        for (auto scope = scopes.rbegin(); scope != scopes.rend(); ++scope)
        {
            auto isVarOnScope = (*scope)->varExistsInHierarchy(name);
            if (isVarOnScope)
                return (*scope)->getVar(name);
        }

        return current_scope->getVar(name);
    }

    void updateVar(const std::string &name, const Value &value)
    {
        // FIXME: speed it up:
        //  1 - Store scope level 'enum class ScopeLevel { PARAMS, INSTANCE_ATTR, GLOBAL };' at parseTime on GetParamExpr|GetAttributeExpr|GetGlobalVarExpr
        //  2 - store an index into the Heart (like int slot) at parse time, so getVar doesn’t even need to hash the 'name' at runtime — just array access.

        // Iterate over the scopes in reverse order (rbegin, rend)
        for (auto scope = scopes.rbegin(); scope != scopes.rend(); ++scope)
        {
            auto isVarOnScope = (*scope)->varExistsInHierarchy(name);
            if (isVarOnScope)
            {
                (*scope)->updateVar(name, value);

                return;
            }
        }

        current_scope->updateVar(name, value);
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

    void registerVarType(const std::string &type, const std::string &name)
    {
        current_scope->registerVarType(type, name);
    }

    void registerClassName(const std::string &className)
    {
        current_scope->registerClassName(className);
    }
};
