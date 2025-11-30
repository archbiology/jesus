#pragma once

#include <string>
#include <vector>
#include "uml_field_info.hpp"
#include "../../utils/string_utils.hpp"

struct UmlClassBuilder
{
    std::string name;
    std::string package;
    std::vector<std::string> parents;
    std::vector<UmlFieldInfo> fields;
    std::vector<std::string> methods;
    /**
     * @brief Classes used by a given class.
     */
    std::vector<std::string> uses;

    UmlClassBuilder &packageName(std::string pkg)
    {
        package = pkg;
        return *this;
    }

    UmlClassBuilder &parentsList(std::initializer_list<std::string> p)
    {
        parents = p;
        return *this;
    }

    UmlClassBuilder &fieldsList(const std::vector<std::string> &rawFields)
    {
        for (const auto &f : rawFields)
        {
            auto pos = f.find(':');
            UmlFieldInfo info;

            if (pos == std::string::npos)
            {
                info.name = f;
                info.type = ""; // no type given
            }
            else
            {
                info.name = utils::trim(f.substr(0, pos));
                info.type = utils::trim(f.substr(pos + 1));
            }
            fields.push_back(info);
        }
        return *this;
    }

    UmlClassBuilder &methodsList(std::initializer_list<std::string> m)
    {
        methods = m;
        return *this;
    }

    UmlClassBuilder &usesList(std::initializer_list<std::string> u)
    {
        for (auto &x : u)
            uses.push_back(utils::trim(x));

        return *this;
    }
};
