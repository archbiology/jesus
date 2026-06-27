#pragma once

#include <memory>
#include <string>
#include <unordered_map>
#include "creation_type.hpp"

/**
 * @brief Parse-time registry of known types.
 *
 * Stores both built-in types (number, text, etc.) and user-defined
 * classes so that the parser can resolve types before execution.
 *
 * It was introduced when implementing class and instance
 * support for the VM. Previously, user-defined classes only existed
 * after being executed by the AST interpreter (REPL mode), meaning
 * constructs such as:
 *
 *     let there be Light: amen
 *     create Light jesus = 1
 *
 * were accepted in the REPL but failed when compiling an entire file
 * (e.g. `jesus file.jesus` or `jesus --vm file.jesus`), because the
 * class registration had not yet been executed.
 *
 * "The Spirit searches all things, even the deep things of God."
 * — 1 Corinthians 2:10
 */
class TypeRegistry
{
public:
    void registerType(std::shared_ptr<CreationType> type)
    {
        types[type->name] = type;
    }

    std::shared_ptr<CreationType> resolve(const std::string &name) const
    {
        auto it = types.find(name);
        if (it != types.end())
            return it->second;

        return nullptr;
    }

    bool exists(const std::string &name) const
    {
        return types.contains(name);
    }

private:
    std::unordered_map<std::string, std::shared_ptr<CreationType>> types;
};
