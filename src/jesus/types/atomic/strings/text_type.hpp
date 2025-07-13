#pragma once

#include "../../creation_type.hpp"

class TextType : public CreationType
{
public:
    TextType() : CreationType("text", "core") {}

    bool validate(const Value &value) const override
    {
        return value.IS_STRING;
    }
};
