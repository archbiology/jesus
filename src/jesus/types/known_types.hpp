#pragma once
#include <string>
#include <unordered_map>
#include "creation_type.hpp"
#include "../spirit/value.hpp"

class KnownTypes
{
public:
    static void registerBuiltInTypes();

    static void registerType(std::shared_ptr<CreationType> type);

    static const std::shared_ptr<CreationType> resolve(const std::string &name, const std::string &module = "core");
    static const CreationType *getById(int id);

    static bool isValid(const CreationType *type, const Value &value);

    // -------------------------------------------------------------------
    // Direct access for common types for fast type analysis at parse time
    // -------------------------------------------------------------------
    inline static std::shared_ptr<CreationType> SEX;
    inline static std::shared_ptr<CreationType> BORN;

    inline static std::shared_ptr<CreationType> TRUTH;
    inline static std::shared_ptr<CreationType> BOOLEAN;

    inline static std::shared_ptr<CreationType> WEEKDAY;

    inline static std::shared_ptr<CreationType> INT;
    inline static std::shared_ptr<CreationType> NATURAL; // unsigned int
    inline static std::shared_ptr<CreationType> FLOAT;
    inline static std::shared_ptr<CreationType> DOUBLE;

    inline static std::shared_ptr<CreationType> STRING;
    // -------------------------------------------------------------------

private:
    /**
     * @brief  Joins module and type name (e.g. core.born)
     */
    static std::string makeKey(const std::string &module_name, const std::string &name);

    inline static std::unordered_map<std::string, std::shared_ptr<CreationType>> typesByName;
    inline static std::vector<const CreationType *> typesById;
};
