#pragma once

#include <unordered_map>
#include "../types/creation_type.hpp"
#include "../types/known_types.hpp"

class SemanticAnalyzer
{
public:
    static void registerVarType(const std::string &varName, const std::string &typeName)
    {
        variableTypes[varName] = typeName;
    }

    static const CreationType *getVarType(const std::string &varName)
    {

        auto it = variableTypes.find(varName);
        if (it == variableTypes.end())
            return nullptr;

        std::string varTypeStr = it->second;

        const CreationType *varType = KnownTypes::resolve(varTypeStr);
        return varType;
    }

private:
    inline static std::unordered_map<std::string, std::string> variableTypes; // varName: typeName
};
