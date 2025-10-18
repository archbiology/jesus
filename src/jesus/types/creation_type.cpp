#include "creation_type.hpp"
#include "../parser/helpers/member.hpp"

std::shared_ptr<Member> CreationType::findMember(const std::string &name, std::shared_ptr<CreationType> current_class) const
{
    std::shared_ptr<CreationType> klass = current_class;

    while (klass)
    {
        // ----------------
        // Attribute lookup
        // ----------------
        if (klass->class_attributes && klass->class_attributes->varExists(name))
            return std::make_shared<Member>(name, klass);

        // -------------
        // Method lookup
        // -------------
        auto methodIt = klass->methods.find(name);
        if (methodIt != klass->methods.end())
            return std::make_shared<Member>(methodIt->second, klass);

        // -------------------------------------------
        // Not found. Let's search in the parent class
        // -------------------------------------------
        klass = klass->parent_class;
    }

    return nullptr;
}
