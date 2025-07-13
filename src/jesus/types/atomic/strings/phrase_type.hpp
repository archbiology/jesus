#pragma once

#include "../../creation_type.hpp"

class PhraseType : public CreationType
{
public:
    PhraseType() : CreationType("phrase", "core") {}

    bool validate(const Value &value) const override
    {
        return value.IS_STRING;
    }
};
