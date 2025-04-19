#include "spirit/heart.hpp"

void Heart::set(const std::string &name, const std::string &value)
{
    variables[name] = value;
}

std::optional<std::string> Heart::get(const std::string &name) const
{
    auto it = variables.find(name);
    if (it != variables.end())
    {
        return it->second;
    }

    return std::nullopt;
}
