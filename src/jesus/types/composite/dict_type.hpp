#pragma once

#include "types/creation_type.hpp"

class Interpreter; // Forward declaration

/**
 * @brief Runtime representation of dict<Key,Value>.
 */
class DictType : public CreationType
{
public:
    std::shared_ptr<CreationType> keyType;
    std::shared_ptr<CreationType> valueType;

    DictType(
        std::shared_ptr<CreationType> keyType,
        std::shared_ptr<CreationType> valueType,
        std::shared_ptr<CreationType> parent,
        std::string name = "dict")
        : CreationType(PrimitiveType::Collection, name, "core", parent),
          keyType(keyType), valueType(valueType)
    {
        registerMethods();
    }

    bool validate(const Value &value) const override
    {
        if (!value.IS_DICT)
            return false;

        for (const auto &[k, v] : value.asDict())
        {
            if (!keyType->validate(*k))
                return false;

            if (!valueType->validate(*v))
                return false;
        }

        return true;
    }

private:
    void registerMethods();

    static Value contains(Interpreter &, Value &dict, const std::vector<Value> &args);

    static Value remove(Interpreter &, Value &dict, const std::vector<Value> &args);

    static Value count(Interpreter &, Value &dict, const std::vector<Value> &args);

    static Value isEmpty(Interpreter &, Value &dict, const std::vector<Value> &args);

    static Value clear(Interpreter &, Value &dict, const std::vector<Value> &args);

    static Value keys(Interpreter &, Value &dict, const std::vector<Value> &args);

    static Value values(Interpreter &, Value &dict, const std::vector<Value> &args);

    static Value pairs(Interpreter &, Value &dict, const std::vector<Value> &args);

    static Value get(Interpreter &, Value &dict, const std::vector<Value> &args);

    static Value copy(Interpreter &, Value &dict, const std::vector<Value> &args);

    static Value update(Interpreter &, Value &dict, const std::vector<Value> &args);
};
