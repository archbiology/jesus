#pragma once

#include <iostream>
#include "../spirit/value.hpp"
#include "../understanding/core/REGISTER_FOR_UML.hpp"

struct Heart; // forward declaration

REGISTER_FOR_UML(
    ASTNode,
    .methodsList({"evaluate"}));

/**
 * @brief Abstract base class for all nodes in the Abstract Syntax Tree (AST).
 *
 * ASTNode serves as the foundation for all executable elements in the language.
 * Each derived class must implement the `execute` method, defining its behavior
 * when evaluated or run by the interpreter.
 *
 * The use of a virtual destructor ensures proper cleanup when deleting derived
 * nodes through a base class pointer.
 *
 * AST nodes hold structure, Interpreter holds behavior.
 *
 * "Let all things be done decently and in order." — 1 Corinthians 14:40
 */
class ASTNode
{
public:
    virtual ~ASTNode() = default;

    /**
     * @brief Evaluates the node and returns an optional value.
     * Defaults to no value, meant to be overridden in subclasses that produce a result.
     *
     * "The Spirit searches all things, even the deep things of God." — 1 Corinthians 2:10
     *
     * @param heart Runtime context containing variables and their values.
     * @return Value The result of the evaluation, or monostate if not applicable.
     */
    virtual Value evaluate(Heart *heart)
    {

        std::cout << "🔴️ ASTNode.evaluate(should be implemented on child classes)\n";
        return Value::formless();
    }

    virtual size_t approxSize() const { return sizeof(*this); }

    /**
     * @brief Returns a string representation of the node.
     *
     * "For nothing is hidden that will not be made manifest, nor is anything
     * secret that will not be known and come to light." — Luke 8:17
     */
    virtual std::string toString() const { return typeid(*this).name(); }
};

/**
 * @brief Output stream overload for ASTNode
 *
 * Uses the node's `toString()` method to produce a readable description.
 *
 * "Therefore, if anyone is in Christ, he is a new creation. The old has passed away; behold, the new has come." — 2 Corinthians 5:17
 */
inline std::ostream &operator<<(std::ostream &os, const ASTNode &node)
{
    return os << node.toString();
}
