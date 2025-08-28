#pragma once

#include <unordered_map>
#include <unordered_set>
#include "../types/creation_type.hpp"
#include "../types/known_types.hpp"

class SemanticAnalyzer
{
public:
    static void registerClassName(const std::string &className)
    {
        classNames.insert(className);
    }

    static void registerVarType(const std::string &varName, const std::string &typeName)
    {
        variableTypes[varName] = typeName;
    }

    static const std::shared_ptr<CreationType> getVarType(const std::string &varName)
    {

        auto it = variableTypes.find(varName);
        if (it == variableTypes.end())
            return nullptr;

        std::string varTypeStr = it->second;

        auto varType = KnownTypes::resolve(varTypeStr);
        return varType;
    }

    static bool isClassKnown(const std::string &className)
    {
        return classNames.find(className) != classNames.end();
    }

private:
    inline static std::unordered_map<std::string, std::string> variableTypes; // varName: typeName
    inline static std::unordered_set<std::string> classNames;
};
