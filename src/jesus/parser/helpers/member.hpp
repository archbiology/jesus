#pragma once

#include <string>

class Method;       // Forward declaration
class CreationType; // Forward declaration

/**
 * @brief Represents a class member (attribute or method) during parsing/type-checking.
 *
 * Only used by the parser; does not affect runtime.
 *
 * "Test everything; hold fast what is good." — 1 Thessalonians 5:21
 */
class Member
{
public:
    const std::string attr_name;                         // non-empty if it’s an attribute
    const std::shared_ptr<Method> method;                // non-null if it’s a method
    const std::shared_ptr<CreationType> declaring_class; // who declared it

    Member(std::string attr, std::shared_ptr<CreationType> klass)
        : attr_name(std::move(attr)), declaring_class(std::move(klass)) {}

    Member(std::shared_ptr<Method> method, std::shared_ptr<CreationType> klass)
        : method(std::move(method)), declaring_class(std::move(klass)) {}

    bool isAttribute() const { return !attr_name.empty(); }

    bool isMethod() const { return method != nullptr; }
};
