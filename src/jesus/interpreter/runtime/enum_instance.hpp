#pragma once

#include "instance.hpp"

class EnumInstance : public Instance
{
  public:
    const int value;
    const std::string memberName;
    const std::string label;

    EnumInstance(
        std::shared_ptr<CreationType> enumType, const std::string &memberName, const std::string &label, int value)
        : Instance(enumType), value(value), memberName(memberName), label(label)
    {
    }

    std::string toString() const override
    {
        return memberName;
    }
};
