#include "../../types/creation_type.hpp"

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
     *
     * "The Spirit gives life; the flesh counts for nothing.
     * The words I have spoken to you—they are full of the Spirit[a] and life."
     * — John 6:63
     */
    CreationType spirit;

    /**
     * @brief Construct a new Instance with a given spirit (class).
     * @param klass The class (spirit) that defines this instance.
     */
    explicit Instance(CreationType klass)
        : spirit(std::move(klass)) {}

    const Value getAttribute(const std::string &name) const
    {
        return spirit.getAttribute(name);
    }

    std::string toString()
    {
        return "<Instance '" + spirit.name + "' >";
    }
};
