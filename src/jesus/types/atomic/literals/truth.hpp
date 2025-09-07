#pragma once

#include "../../creation_type.hpp"

/**
 * @brief In Jesus Programming Language, the boolean type is called Truth.
 *
 * It represents alignment with truth (yes/no).
 * For compatibility, boolean.hpp exists as an alias.
 *
 * Jesus answered, "I am the way and the truth and the life. No one comes to the Father except through me." — John 14:6
 */
class TruthType : public CreationType
{
public:
    TruthType() : CreationType(PrimitiveType::Boolean, "truth", "core") {}

    bool validate(const Value &value) const override
    {
        return value.IS_BOOLEAN;
    }
};
