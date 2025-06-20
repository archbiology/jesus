#pragma once

#include "../ast/stmt/stmt.hpp"

/**
 * @brief The RevealNode class represents the `reveal` command in the language.
 *
 * This node is responsible for retrieving and displaying the value of a variable
 * previously declared in the execution context (the Heart). If the variable exists,
 * its value is printed; otherwise, a message indicates that it is unknown.
 *
 * "He reveals deep and hidden things; He knows what is in the darkness, and light dwells with Him." — Daniel 2:22
 *
 * @example
 * (Jesus) let there be name set to Jesus
 * name = Jesus
 * (Jesus) say name
 * Jesus
 * (Jesus) warn name
 * Jesus
 */
struct RevealNode : public Stmt
{
    ASTNode *node;

    /**
     * @brief Construct a new RevealNode object
     *
     * "Call to me and I will answer you and tell you great and unsearchable things you do not know."
     * — Jeremiah 33:3
     *
     * @param id A pointer to an ASTNode representing a variable or a value.
     */
    explicit RevealNode(ASTNode *node) : node(node) {}

    /**
     * @brief Returns a string representation of the node.
     *
     * "For nothing is hidden that will not be made manifest, nor is anything
     * secret that will not be known and come to light." — Luke 8:17
     */
    std::string toString() const override
    {
        std::string str = "RevealNode";

        if (node)
            str += "(" + node->toString() + ")";

        return str;
    }
};
