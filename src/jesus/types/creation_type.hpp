#pragma once
#include <string>
#include "../spirit/value.hpp"
#include "constraints/constraint.hpp"

enum class PrimitiveType
{
    Class,
    Number,
    Text
};

class CreationType
{

public:
    virtual ~CreationType() = default;

    /**
     * @brief Unique internal ID for runtime (index in vector)
     */
    const int id;

    /**
     * @brief The creation type. E.g.: number, User
     */
    const std::string name;

    /**
     * @brief The module name. E.g.: core, my.custom.package
     */
    const std::string module_name;

    const PrimitiveType primitive_type;

    CreationType(PrimitiveType primitive_type, std::string name, std::string module = "core",
                 std::vector<std::shared_ptr<IConstraint>> constraints = {})
        : primitive_type(primitive_type), name(name), module_name(module), constraints(std::move(constraints)), id(lastId++) {}

    virtual Value parseFromString(const std::string &raw) const
    {
        switch (primitive_type)
        {
        case PrimitiveType::Number:
        {
            try
            {
                double number = std::stod(raw);
                return Value(number);
            }
            catch (...)
            {
                throw std::runtime_error("Invalid number: '" + raw + "'");
            }
        }

        case PrimitiveType::Text:
            return Value(raw);

        default:
            throw std::runtime_error("Unknown primitive type for parsing.");
        }
    }

    std::string primitiveTypeName() const
    {
        switch (primitive_type)
        {
        case PrimitiveType::Class:
            return "creation";
        case PrimitiveType::Number:
            return "number";
        case PrimitiveType::Text:
            return "text";
        default:
            return "unknown";
        }
    }

    /**
     * @brief Returns true if the value belongs to this type.
     */
    bool virtual validate(const Value &value) const
    {
        for (const auto &c : constraints)
            c->validate(value); // may throw

        return true;
    }

private:
    inline static int lastId = 0;

    std::vector<std::shared_ptr<IConstraint>> constraints;
};
