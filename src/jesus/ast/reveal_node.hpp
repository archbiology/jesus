#pragma once

#include "ast_node.hpp"
#include "../spirit/heart.hpp"

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
struct RevealNode : public ASTNode
{
    ASTNode *node;

    /**
     * @brief Construct a new RevealNode object
     *
     * @param id A pointer to an ASTNode representing a variable or a value.
     */
    explicit RevealNode(ASTNode *node) : node(node) {}

    /**
     * @brief Executes the `say` command by displaying the value of `node`.
     *
     * This method queries the Heart (symbol table) for the variable associated with
     * the provided identifier. If found, it prints the value; otherwise, it prints
     * a message indicating that the variable is unknown.
     *
     * "Call to me and I will answer you and tell you great and unsearchable things you do not know."
     * — Jeremiah 33:3
     *
     * @param heart A pointer to the Heart environment that stores all declared variables.
     */
    void execute(Heart *heart) override
    {
        if (!node)
            return;

        node->execute(heart);
    }

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
