#pragma once

#include "ast_node.hpp"
#include "identifier_node.hpp"
#include "value_node.hpp"
#include "../spirit/heart.hpp"
#include <memory>
#include <iostream>

/**
 * @brief AST node representing a variable declaration and assignment.
 *
 * The LetThereBeNode models a statement like:
 * ```
 * let there be light set to "Jesus";
 * ```
 *
 * It pairs an identifier (the name of the variable) with a value.
 * During execution, it simply prints the assignment to standard output.
 *
 * In a more advanced implementation, this would also register the variable
 * in an environment or symbol table for later retrieval.
 *
 * "And God said, 'Let there be light,' and there was light." — Genesis 1:3
 * This node reflects that same pattern: a spoken declaration resulting in
 * something coming into existence.
 */
struct LetThereBeNode : ASTNode
{
    /**
     * @brief The identifier being declared (e.g., a variable name).
     */
    std::unique_ptr<IdentifierNode> identifier;

    /**
     * @brief The value being assigned to the identifier.
     */
    std::unique_ptr<ValueNode> value;

    /**
     * @brief Constructs a LetThereBeNode with the given identifier and value.
     *
     * @param id The identifier to bind.
     * @param val The value to assign to the identifier.
     */
    LetThereBeNode(std::unique_ptr<IdentifierNode> id, std::unique_ptr<ValueNode> val)
        : identifier(std::move(id)), value(std::move(val)) {}

    /**
     * @brief Executes the node by printing the declaration.
     *
     * If the value is not empty, it is printed in the format:
     * ```
     * name = value
     * ```
     * Otherwise, only the name is printed.
     *
     * @param heart Pointer to the Heart (Symbol table) for variable storage.
     */
    void execute(Heart *heart) override
    {
        std::cout << identifier->name;

        if (!std::holds_alternative<std::monostate>(value->value))
        {
            std::cout << " = " << std::visit(make_string_functor(), value->value);

            if (heart)
            {
                heart->assign(identifier->name, value->value);
            }
        }

        std::cout << std::endl;
    }

    /**
     * @brief Returns a string representation of the node.
     *
     * "For nothing is hidden that will not be made manifest, nor is anything
     * secret that will not be known and come to light." — Luke 8:17
     */
    std::string toString() const override  {

        std::string str = "LetThereBeNode";

        if (identifier) str += "(" + identifier->toString();
        if (value) str += ", " + value->toString();

        str += ")";

        return str;


    }
};
