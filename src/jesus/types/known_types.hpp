#pragma once
#include <string>
#include <unordered_map>
#include "creation_type.hpp"
#include "../spirit/value.hpp"

class KnownTypes
{
public:
    static void registerBuiltInTypes();

    static void registerType(std::unique_ptr<CreationType> type);

    static const CreationType *resolve(const std::string &name, const std::string &module = "core");
    static const CreationType *getById(int id);

    static bool isValid(const CreationType *type, const Value &value);

private:
    /**
     * @brief  Joins module and type name (e.g. core.born)
     */
    static std::string makeKey(const std::string &module_name, const std::string &name);

    inline static std::unordered_map<std::string, std::unique_ptr<CreationType>> typesByName;
    inline static std::vector<const CreationType *> typesById;
};
