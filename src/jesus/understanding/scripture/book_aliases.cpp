#include "book_aliases.hpp"
#include <iostream>

namespace HolyBible
{

    std::unordered_map<std::string, HolyBible::BookId> buildAliasMap()
    {
        std::unordered_map<std::string, HolyBible::BookId> map;

        for (const auto &group : BOOK_ALIAS_GROUPS)
        {
            for (const auto &alias : group.aliases)
            {
                if (map.contains(alias))
                {
                    std::cerr << "Duplicated alias: " << alias << "\n";
                }

                map[alias] = group.id;
            }
        }

        return map;
    }

    const std::unordered_map<std::string, HolyBible::BookId> BOOK_ALIASES = buildAliasMap();
};