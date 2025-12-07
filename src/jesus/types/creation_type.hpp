#pragma once
#include <string>
#include "../spirit/value.hpp"
#include "../ast/expr/expr.hpp"
#include "constraints/constraint.hpp"

class Method; // Forward declaration
class Member; // Forward declaration

enum class PrimitiveType
{
    Null,
    Good, // Genesis 1:31
    Boolean,
    Number,
    Text,
    Module,
    Class,
    Polymorphic,
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
    std::shared_ptr<CreationType> parent_class = nullptr;

    std::shared_ptr<Heart> class_attributes = nullptr;
    std::unordered_map<std::string, std::shared_ptr<Method>> methods;

    CreationType(PrimitiveType primitive_type, std::string name, std::string module = "core",
                 std::shared_ptr<CreationType> parent = nullptr,
                 std::vector<std::shared_ptr<IConstraint>> constraints = {})
        : primitive_type(primitive_type),
          name(name), module_name(module),
          parent_class(std::move(parent)),
          constraints(std::move(constraints)), id(lastId++)
    {

        if (primitive_type == PrimitiveType::Class)
        {
            std::shared_ptr<Heart> parent_attributes = nullptr;
            if (parent_class)
                parent_attributes = parent_class->class_attributes;

            class_attributes = std::make_shared<Heart>(name, parent_attributes);
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

    void addAttribute(const VarType &type, const std::string &name, std::unique_ptr<Expr> initializer, std::shared_ptr<Heart> heart)
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

    /**
     * @brief
     * During attribute resolution, if a member is found in the instance or
     * inherited from a parent, a shared pointer to this Member is returned.
     * Otherwise, `nullptr` is returned to indicate the absence of a definition.
     *
     * @param name
     * @return std::variant<std::shared_ptr<Method>, std::string>
     */
    virtual std::shared_ptr<Member> findMember(const std::string &name, std::shared_ptr<CreationType> current_class) const;

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

    const bool isPolymorphic() const
    {
        return primitive_type == PrimitiveType::Polymorphic;
    }

    virtual const bool isException() const
    {
        return false;
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

    /**
     * @brief Checks whether this type is the same as or a child of another type.
     *
     * This method determines if the current type is the same as, or a subclass of,
     * the provided `typeToCheck`. It traverses the inheritance chain, allowing
     * polymorphic checks such as "person is a Human?".
     *
     * "If you were sons of Abraham, you would do the works of Abraham; ...
     *  You belong to your father, the devil,...
     *  He was a murderer from the beginning,
     *  not holding to the truth, for there is no truth in him.
     *  When he lies, he speaks his native language,
     *  for he is a liar and the father of lies." — John 8:39-44 (paraphrased)
     */
    virtual bool isA(const std::shared_ptr<CreationType> &typeToCheck) const
    {
        if (!typeToCheck)
            return false;

        // -----------------------------------
        // Same exact type (by memory address)
        // -----------------------------------
        if (this == typeToCheck.get())
            return true;

        // ---------------------------
        // Climb the inheritance chain
        // ---------------------------
        std::shared_ptr<CreationType> current = parent_class;
        while (current)
        {
            if (current.get() == typeToCheck.get())
                return true;

            current = current->parent_class;
        }

        return false;
    }

    const std::string toString()
    {
        return "{class: \"" + name + "\"}";
    }

    const std::string methodsToString()
    {
        std::string str;
        if (!methods.empty())
        {
            for (auto &p : methods)
            {
                str += p.first + "\n";
            }
        }
        return str;
    }

private:
    inline static int lastId = 0;

    std::vector<std::shared_ptr<IConstraint>> constraints;
};
