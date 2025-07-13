#pragma once

#include "../../creation_type.hpp"

class WordType : public CreationType
{
public:
    WordType() : CreationType("word", "core") {}

    bool validate(const Value &value) const override
    {
        return value.IS_STRING;
    }
};
