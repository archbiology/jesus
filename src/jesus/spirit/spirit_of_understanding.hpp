#pragma once

#include <unordered_map>
#include <unordered_set>
#include <string>
#include <memory>
#include "spirit_of_the_lord.hpp"

class CreationType; // Forward declaration

using VarType = std::shared_ptr<CreationType>;
/**
 * @brief Understanding/Faith brings Discernment
 *
 * Used as the Semantics Analyzer
 *
 * "The Spirit of the Lord will rest on him —
 * the Spirit of wisdom and of understanding,
 * the Spirit of counsel and of might,
 * the Spirit of the knowledge and fear of the Lord" — Isaiah 11:2
 */
class SpiritOfUnderstanding : SpiritOfTheLord
{
public:
    void registerClassName(const std::string &className)
    {
        classNames.insert(className);
    }

    void registerVarType(const VarType &type, const std::string &name)
    {
        variableTypes[name] = type;
    }

    void updatePolymorphicVarType(const std::string& name, const VarType &newType){
        variableTypes[name] = newType;
    }

    const std::shared_ptr<CreationType> getVarType(const std::string &varName);

    bool isClassKnown(const std::string &className)
    {
        return classNames.find(className) != classNames.end();
    }

    std::string toString();

private:
    std::unordered_map<std::string, VarType> variableTypes; // varName: varType
    std::unordered_set<std::string> classNames;
};
