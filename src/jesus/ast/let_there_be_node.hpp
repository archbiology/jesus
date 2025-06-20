#pragma once

#include "../ast/stmt/stmt.hpp"
#include "../ast/expr/literal_expr.hpp"
#include "identifier_node.hpp"
#include <memory>

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
struct LetThereBeNode : Stmt
{
    /**
     * @brief The identifier being declared (e.g., a variable name).
     */
    std::unique_ptr<IdentifierNode> identifier;

    /**
     * @brief The value being assigned to the identifier.
     */
    std::unique_ptr<LiteralExpr> value;

    /**
     * @brief Constructs a LetThereBeNode with the given identifier and value.
     *
     * @param id The identifier to bind.
     * @param val The value to assign to the identifier.
     */
    LetThereBeNode(std::unique_ptr<IdentifierNode> id, std::unique_ptr<LiteralExpr> val)
        : identifier(std::move(id)), value(std::move(val)) {}

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
