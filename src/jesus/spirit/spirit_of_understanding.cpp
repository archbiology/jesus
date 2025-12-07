#include "../types/creation_type.hpp"
#include "spirit_of_understanding.hpp"

const std::shared_ptr<CreationType> SpiritOfUnderstanding::getVarType(const std::string &varName)
{

    auto it = variableTypes.find(varName);
    if (it == variableTypes.end())
        return nullptr;

    return it->second;
}

std::string SpiritOfUnderstanding::toString()
{
    std::cout << " Understanding::variableTypes: \n";
    for (auto &pair : variableTypes)
    {
        const std::string &key = pair.first;
        const std::string &value = pair.second->name;

        std::cout << "  '" << value << "':= '" << key << "';\n";
    }

    std::cout << " Understanding::classNames: \n";
    for (auto &name : classNames)
    {
        std::cout << "  '" << name << "'\n";
    }
    return "";
}
