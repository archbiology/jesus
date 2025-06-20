#pragma once

#include "../ast/expr/expr.hpp"
#include <string>

/**
 * @brief AST node representing a literal value.
 *
 * The ValueNode holds a raw value such as a string or number
 * that can be used in expressions, assignments, or function calls.
 *
 * In a full interpreter, this node might return the value or push it
 * onto an evaluation stack.
 *
 * "Every good and perfect gift is from above..." — James 1:17
 *  A ValueNode reminds us that even simple values can carry meaning and purpose.
 */
struct ValueNode : Expr
{
    /**
     * @brief The literal value held by this node.
     */
    Value value;

    /**
     * @brief Constructs a ValueNode with the given literal value.
     *
     * @param value The raw string value.
     */
    ValueNode(const Value &value) : value(value) {}

    Value evaluate(Heart *heart) override
    {
        return value;
    }

    std::string toString() const override { return "ValueNode(" + value.toString() + ")"; }

    bool isNumber() {
        return value.IS_NUMBER;
    }

    bool isString() {
        return value.IS_STRING;
    }

    bool isBoolean() {
        return value.IS_BOOLEAN;
    }

    bool isFormless() {
        return value.IS_FORMLESS;
    }
};
