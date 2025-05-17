// #include "../spirit/heart.hpp"
#include "heart.hpp"
#include <stdexcept>

// void Heart::set(const std::string &name, const std::string &value)
// {
//     variables[name] = value;
// }

void Heart::define(const std::string &name, const Value &value)
{
    variables[name] = value;
}

// std::optional<std::string> Heart::get(const std::string &name) const
Value Heart::get(const std::string &name) const
{
    auto it = variables.find(name);
    if (it == variables.end())
    {
        throw std::runtime_error("Undefined variable: " + name);
    }
    return it->second;

    // auto it = variables.find(name);
    // if (it != variables.end())
    // {
    //     return it->second;
    // }

    // return std::nullopt;
}

void Heart::assign(const std::string &name, const Value &value)
{
    // auto it = variables.find(name);
    // if (it == variables.end())
    // {
    //     throw std::runtime_error("Cannot assign to undefined variable: " + name);
    // }

    variables[name] = value;
}
