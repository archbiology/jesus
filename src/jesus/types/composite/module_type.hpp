#pragma once

#include "../creation_type.hpp"

/**
 * @brief Represents a module type in the language.
 *
 * "We have different gifts, according to the grace given to each of us.
 * If your gift is prophesying, then prophesy in accordance with your faith;"
 * — Romans 12:6
 */
class ModuleType : public CreationType
{
public:
    ModuleType(std::shared_ptr<CreationType> parent)
        : CreationType(PrimitiveType::Module, "module", "core", parent) {}

    bool validate(const Value &value) const override
    {
        return value.IS_MODULE;
    }
};
