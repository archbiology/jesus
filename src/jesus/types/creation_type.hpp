#pragma once
#include <string>
#include "../spirit/value.hpp"
#include "../ast/expr/expr.hpp"
#include "constraints/constraint.hpp"

class Method; // Forward declaration

enum class PrimitiveType
{
    Null,
    Boolean,
    Number,
    Text,
    Class
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

    std::shared_ptr<Heart> class_attributes = nullptr;
    std::unordered_map<std::string, std::shared_ptr<Method>> methods;

    CreationType(PrimitiveType primitive_type, std::string name, std::string module = "core",
                 std::vector<std::shared_ptr<IConstraint>> constraints = {})
        : primitive_type(primitive_type), name(name), module_name(module), constraints(std::move(constraints)), id(lastId++)
    {

        if (primitive_type == PrimitiveType::Class)
        {
            class_attributes = std::make_shared<Heart>(name);
        }
    }

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

    void addAttribute(const std::string &type, const std::string &name, std::unique_ptr<Expr> initializer, std::shared_ptr<Heart> heart)
    {
        if (primitive_type != PrimitiveType::Class)
            throw std::runtime_error("Only class types can have attributes.");

        Value initVal;
        if (initializer)
        {
            // Evaluate the expression to a Value (optional; could be done lazily)
            initVal = initializer->evaluate(heart);
        }

        class_attributes->createVar(type, name, initVal);
    }

    const Value getAttribute(const std::string &name) const
    {
        return class_attributes->getVar(name);
    }

    void addMethod(const std::string &name, std::shared_ptr<Method> method)
    {
        if (primitive_type != PrimitiveType::Class)
            throw std::runtime_error("Only class types can have methods.");

        methods[name] = std::move(method);
    }

    bool attributeExists(const std::string &name)
    {
        if (primitive_type == PrimitiveType::Class)
        {
            return class_attributes->varExists(name);
        }
        return false;
    }

    std::shared_ptr<Method> findMethod(const std::string &name) const
    {
        auto it = methods.find(name);
        if (it != methods.end())
            return it->second;

        return nullptr;
    }

    const bool isNull() const
    {
        return primitive_type == PrimitiveType::Null;
    }

    const bool isVoid() const
    {
        return isNull();
    }

    const bool isBoolean() const
    {
        return primitive_type == PrimitiveType::Boolean;
    }

    const bool isNumber() const
    {
        return primitive_type == PrimitiveType::Number;
    }

    const bool isString() const
    {
        return primitive_type == PrimitiveType::Text;
    }

    const bool isClass() const
    {
        return primitive_type == PrimitiveType::Class;
    }

    /**
     * @brief Checks if this CreationType can accept a value of another type.
     *
     * This function determines whether the current CreationType is compatible
     * with the provided `assigningValueType`, i.e., whether a value of that
     * type can be safely or meaningfully assigned to this one.
     *
     * "Can two walk together, except they be agreed?" — Amos 3:3 (KJV)
     *
     * @param assigningValueType A shared pointer to the CreationType that represents
     *      the type of value being assigned.
     * @return true if the assignment is considered valid; false otherwise.
     */
    bool isCompatibleWith(const std::shared_ptr<CreationType> &assigningValueType) const
    {
        if (!assigningValueType)
            return false;

        // -------------------------------
        // Same exact type (by id or name)
        // -------------------------------
        if (this->id == assigningValueType->id)
            return true;

        if (this->name == assigningValueType->name && this->module_name == assigningValueType->module_name)
            return true;

        // ----------------------------------------------------------------------
        // Null assignability: allow assigning "null" to any reference/class type
        // ----------------------------------------------------------------------
        if (this->isClass() && assigningValueType->isNull())
            return true;

        // --------------------------------
        // Primitive compatibility (number)
        // --------------------------------
        if (this->isNumber() && assigningValueType->isNumber())
            return true;

        if (this->isString() && assigningValueType->isString())
            return true;

        if (this->isBoolean() && assigningValueType->isBoolean())
            return true;

        // Class assignability (TODO: add inheritance checks)
        if (this->isClass() && assigningValueType->isClass())
        {
            // TODO: Add subclassing logic
            return false;
        }

        return false;
    }

    const std::string toString()
    {
        return "{class: \"" + name + "\"}";
    }

private:
    inline static int lastId = 0;

    std::vector<std::shared_ptr<IConstraint>> constraints;
};
