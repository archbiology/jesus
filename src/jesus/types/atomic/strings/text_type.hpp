#pragma once

#include "../../creation_type.hpp"

class TextType : public CreationType
{
public:
    TextType(std::shared_ptr<CreationType> parent)
        : CreationType(PrimitiveType::Text, "text", "core", parent) {}

    bool validate(const Value &value) const override
    {
        return value.IS_STRING;
    }
};
