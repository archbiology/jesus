#pragma once

#include "uml_class_info.hpp"
#include "uml_class_builder.hpp"
#include <map>
#include <string>

class UmlRegistry
{
public:
    static UmlRegistry &instance()
    {
        static UmlRegistry inst;
        return inst;
    }

    const std::map<std::string, UmlClassInfo> &allClasses() const
    {
        return classes;
    }

    void registerDescriptor(const UmlClassBuilder &desc)
    {
        classes[desc.name] = UmlClassInfo{
            desc.name,
            desc.package,
            desc.parents,
            desc.fields,
            desc.methods,
            desc.uses,
        };
    }

private:
    UmlRegistry() = default;

    std::map<std::string, UmlClassInfo> classes;
};
