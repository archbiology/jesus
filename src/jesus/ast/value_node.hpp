#pragma once

#include "ast_node.hpp"
#include <string>

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
    std::string value;

    /**
     * @brief Constructs a ValueNode with the given literal value.
     *
     * @param value The raw string value.
     */
    ValueNode(const std::string &value) : value(value) {}

    /**
     * @brief Executes the node (currently does nothing).
     */
    void execute() override {}
};
