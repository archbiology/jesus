#pragma once

#include "ast_node.hpp"
#include "identifier_node.hpp"
#include "spirit/heart.hpp"
#include <iostream>

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
 * (Jesus) reveal name
 * Jesus
 * (Jesus)
 */
struct RevealNode : public ASTNode
{
    IdentifierNode *identifier;

    /**
     * @brief Construct a new RevealNode object
     *
     * @param id A pointer to an IdentifierNode representing the variable name.
     */
    RevealNode(IdentifierNode *id) : identifier(id) {}

    /**
     * @brief Executes the `reveal` command by displaying the value of a stored variable.
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
        if (!heart)
            return;

        auto value = heart->get(identifier->name);
        if (value.has_value())
        {
            std::cout << value.value() << std::endl;
        }
        else
        {
            std::cout << "Unknown: " << identifier->name << std::endl;
        }
    }
};
