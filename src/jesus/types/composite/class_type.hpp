#pragma once

#include "../creation_type.hpp"

/**
 * @brief Represents a user-defined class type in the language.
 *
 * Just as man is the crown of creation, made in God’s image,
 * classes represent the highest form of creation in the program,
 * shaping structure and behavior.
 *
 * "So God created mankind in his own image,
 * in the image of God he created them;
 * male and female he created them."
 * — Genesis 1:27
 */
class ClassType : public CreationType
{
public:
    ClassType(std::shared_ptr<CreationType> parent)
        : CreationType(PrimitiveType::Class, "class", "core", parent) {}

    ClassType(std::shared_ptr<CreationType> parent, std::string exceptionName)
        : CreationType(PrimitiveType::Class, exceptionName, "core", parent) {}

    bool validate(const Value &value) const override
    {
        return value.IS_INSTANCE;
    }
};
