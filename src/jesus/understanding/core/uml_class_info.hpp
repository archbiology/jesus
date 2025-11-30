#pragma once

#include <string>
#include <vector>
#include "uml_field_info.hpp"

struct UmlClassInfo
{
    std::string name;
    std::string package;
    std::vector<std::string> parents;
    std::vector<UmlFieldInfo> fields;
    std::vector<std::string> methods;
    std::vector<std::string> uses;
};
