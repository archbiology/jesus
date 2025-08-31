#include "../types/creation_type.hpp"
#include "../types/known_types.hpp"
#include "spirit_of_understanding.hpp"

const std::shared_ptr<CreationType> SpiritOfUnderstanding::getVarType(const std::string &varName)
{

    auto it = variableTypes.find(varName);
    if (it == variableTypes.end())
        return nullptr;

    std::string varTypeStr = it->second;

    auto varType = KnownTypes::resolve(varTypeStr); // FIXME: Shouldn't this be per scope?
    return varType;
}
