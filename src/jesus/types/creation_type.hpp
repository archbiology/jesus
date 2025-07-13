#pragma once
#include <string>
#include "../spirit/value.hpp"

class CreationType
{

public:
    virtual ~CreationType() = default;

    /**
     * @brief Unique internal ID for runtime (index in vector)
     */
    const int id;

    /**
     * @brief The creation type. E.g.: number, User
     */
    const std::string name;

    /**
     * @brief The module name. E.g.: core, my.custom.package
     */
    const std::string module_name;

    CreationType(const std::string &name, const std::string &module_name = "core")
        : name(name), module_name(module_name), id(lastId++) {}

    /**
     * @brief Returns true if the value belongs to this type.
     */
    virtual bool validate(const Value &value) const = 0;

private:
    inline static int lastId = 0;
};
