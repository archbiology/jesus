#pragma once

#include "ast_node.hpp"
#include <string>

enum class ValueType {
    STRING,
    DOUBLE,
    INT,
    BOOL,
    NONE
};

/**
 * @brief AST node representing a literal value.
 *
 * The ValueNode holds a raw value such as a string or number
 * that can be used in expressions, assignments, or function calls.
 *
 * In a full interpreter, this node might return the value or push it
 * onto an evaluation stack. For now, `execute()` is a no-op.
 *
 * "Every good and perfect gift is from above..." — James 1:17
 *  A ValueNode reminds us that even simple values can carry meaning and purpose.
 */
struct ValueNode : ASTNode
{
    /**
     * @brief The literal value held by this node.
     */
    Value value;
    ValueType valueType;

    /**
     * @brief Constructs a ValueNode with the given literal value.
     *
     * @param value The raw string value.
     */
    ValueNode(const Value &value) : value(value), valueType(deduceLiteralType(value)) { }

    Value evaluate(Heart *heart) override
    {
        return value;
    }

    /**
     * @brief Executes the node (currently does nothing).
     *
     * @param heart Pointer to the Heart (Symbol table) for variable storage.
     */
    void execute(Heart* heart) override {}

    std::string toString() const override { return "ValueNode(" + std::visit(make_string_functor(), value) + ")"; }

    bool isNumber() {
        return valueType == ValueType::INT || valueType == ValueType::DOUBLE;
    }

    bool isString() {
        return valueType == ValueType::STRING;
    }

    bool isBoolean() {
        return valueType ==  ValueType::BOOL;
    }

    bool isMonostate() {
        return valueType == ValueType::NONE;
    }

private:
    static ValueType deduceLiteralType(const Value& val) {
        if (std::holds_alternative<std::string>(val)) return ValueType::STRING;
        if (std::holds_alternative<double>(val))      return ValueType::DOUBLE;
        if (std::holds_alternative<int>(val))         return ValueType::INT;
        if (std::holds_alternative<bool>(val))        return ValueType::BOOL;

        return ValueType::NONE;
    }
};
