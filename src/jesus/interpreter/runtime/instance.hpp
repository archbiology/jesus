#pragma once

#include "../../types/creation_type.hpp"

static unsigned long long int InstanceID = 0; // FIXME: Vars created inside loop will blow this up

REGISTER_FOR_UML(
    Instance,
    .packageName("interpreter.runtime")
        .fieldsList({"spirit", "attributes"}));

/**
 * @class Instance
 * @brief Represents a living object (an instance) created from a defined spirit (class).
 *
 * Each Instance carries the "spirit" (its CreationType) that defines
 * its attributes and behavior. Through this, the instance reflects
 * the design of its creation.
 *
 * Example:
 * let there be Human:
 *     create text name = "Adam"
 *     create text wife = "Eve"
 *     create number age = 930
 * amen
 *
 * This would create an Instance of "Human" with the defined attributes.
 *
 * @note The term 'spirit' here parallels the blueprint (class) that animates
 * the instance. Just as the spirit gives life, the class defines the
 * nature of the created object.
 *
 * "And the Lord God formed man of the dust of the ground,
 * and breathed into his nostrils the breath of life;
 * and man became a living soul." — Genesis 2:7
 */
class Instance
{
public:
    /**
     * @brief The spirit (class definition/type) of this instance.
     * Shared across all instances of the same class.
     *
     * "The Spirit gives life; the flesh counts for nothing.
     * The words I have spoken to you—they are full of the Spirit[a] and life."
     * — John 6:63
     */
    std::shared_ptr<CreationType> spirit;

    /**
     * @brief Instance attributes.
     * Starts with a copy of the class's default attributes.
     */
    std::shared_ptr<Heart> attributes;

    /**
     * @brief Construct a new Instance with a given spirit (class).
     * @param klass The class (spirit) that defines this instance.
     */
    explicit Instance(std::shared_ptr<CreationType> klass)
        : spirit(std::move(klass))
    {
        // Copy default class attributes into the instance's heart
        attributes = spirit->class_attributes->clone("i-" + std::to_string(InstanceID++));
    }

    const Value getAttribute(const std::string &name) const
    {
        return attributes->getVar(name);
    }

    void setAttribute(const std::string &name, const Value &value)
    {
        attributes->updateVar(name, value);
    }

    std::string toString()
    {
        std::string str = "{type: \"instance\",\n class: \"" + spirit->name + "\",\n attributes: {";
        if (!attributes->isEmpty())
        {
            str += attributes->toString();
        }
        str += " }\n}";
        return str;
    }
};
