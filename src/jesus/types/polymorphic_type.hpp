#pragma once

#include "creation_type.hpp"

/**
 * @brief [Polymorphism] Holds the runtime type assigned to a parent class.
 *
 * "And the Word became flesh and dwelt among us." — John 1:14
 *
 * This class is especially important when performing **type casting**, **dynamic binding**, or
 * **runtime upcasting/downcasting** in a strongly typed environment.
 *
 * Example
 * ```jesus
 *      Parent abraham = ...
 *      Son isaac = ...
 *
 *      abraham = isaac
 *      abraham is a Son  # yes
 * ```
 */
class PolymorphicType : public CreationType
{
public:
    /**
     * @brief The actual runtime type of the object.
     *
     * "You will recognize them by their fruits." — Matthew 7:16
     */
    std::shared_ptr<CreationType> runtime_type;

    /**
     * @brief Constructs a new polymorphic type relationship.
     *
     * "Do not think that I have come to abolish the Law or the Prophets;
     * I have not come to abolish them but to fulfill them." — Matthew 5:17
     *
     * @param base The declared (compile-time) base type.
     * @param actual The actual (runtime) derived type.
     */
    PolymorphicType(std::shared_ptr<CreationType> base,
                    std::shared_ptr<CreationType> actual)
        : CreationType(PrimitiveType::Polymorphic,
                       base->name + " -> " + actual->name,
                       base->module_name, base),
          runtime_type(std::move(actual)) {}

    /**
     * @brief Validates that the runtime type is compatible with the base type.
     *
     * "Test everything; hold fast what is good." — 1 Thessalonians 5:21
     *
     * @param value The value to validate.
     * @return Always returns `true` if compatible; otherwise throws an exception.
     * @throws std::runtime_error If the runtime type is incompatible with the declared base type.
     */
    bool validate(const Value &value) const override
    {
        if (!parent_class->isCompatibleWith(runtime_type))
            throw std::runtime_error("Type " + runtime_type->name +
                                     " is not compatible with " + parent_class->name);
        return true;
    }

    /**
     * @brief Checks whether the runtime type belongs to or inherits from another type.
     *
     * "I and the Father are one." — John 10:30
     */
    bool isA(const std::shared_ptr<CreationType> &typeToCheck) const override
    {
        return runtime_type->isA(typeToCheck);
    }

    /**
     * @brief Finds a member (field or method) by name.
     *
     * "Ask, and it will be given to you; seek, and you will find." — Matthew 7:7
     */
    std::shared_ptr<Member> findMember(const std::string &name, std::shared_ptr<CreationType> current_class) const override
    {
        return runtime_type->findMember(name, runtime_type);
    }
};
