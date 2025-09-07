#pragma once

#include "../../creation_type.hpp"

/**
 * @brief Values: lightday, skyday, treeday, lampday, fishday, walkday, shabbat
 *
 * It represents alignment with the numbers and 'secular' weekday names:
 *  sunday      = lightday  = 1
 *  monday      = skyday    = 2
 *  tuesday     = treeday   = 3
 *  wednesday   = lampday   = 4
 *  thursday    = fishday   = 5
 *  friday      = walkday   = 6
 *  saturday    = shabbat   = 7
 *
 * And God said, "Let there be light,"" and there was light.
 * God saw that the light was good, and he separated the light from the darkness.
 * God called the light "day,"" and the darkness he called "night."
 * And there was evening, and there was morning—the first day. — Genesis 1:3-5
 */
class WeekdayType : public CreationType
{
public:
    WeekdayType() : CreationType(PrimitiveType::Boolean, "weekday", "core") {}

    bool validate(const Value &value) const override
    {
        if (!value.IS_INT)
            return false;

        int number = value.toInt();

        return number >= 1 && number <= 7;
    }
};
