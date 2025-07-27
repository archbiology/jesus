#pragma once

#include "../../creation_type.hpp"

class WordType : public CreationType
{
public:
    WordType() : CreationType(PrimitiveType::Text, "word", "core") {}

    bool validate(const Value &value) const override
    {
        return value.IS_STRING;
    }
};
