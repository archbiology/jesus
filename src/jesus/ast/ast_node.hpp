#pragma once

#include <optional> // for std::optional
#include <iostream>

struct Heart; // forward declaration

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
 * "Let all things be done decently and in order." — 1 Corinthians 14:40
 */
struct ASTNode
{
    virtual ~ASTNode() = default;

    /**
     * @brief Execute the logic associated with this AST node.
     *
     * This method should be overridden in derived classes to define
     * how each specific type of node behaves at runtime.
     *
     * @param heart Pointer to the Heart (Symbol table) for variable storage.
     */
    virtual void execute(Heart *heart) = 0;

    /**
     * @brief Evaluates the node and returns an optional value.
     * Defaults to no value, meant to be overridden in subclasses that produce a result.
     *
     * "The Spirit searches all things, even the deep things of God." — 1 Corinthians 2:10
     *
     * @param heart Runtime context containing variables and their values.
     * @return std::optional<std::string> The result of the evaluation, or nullopt if not applicable.
     */
    virtual std::optional<std::string> evaluate(Heart *heart)
    {

        std::cout << "🔴️ ASTNode.evaluate(should be implemented on child classes)\n";
        return std::nullopt;
    }

    /**
     * @brief Returns a string representation of the node.
     *
     * "For nothing is hidden that will not be made manifest, nor is anything
     * secret that will not be known and come to light." — Luke 8:17
     */
    virtual std::string toString() const { return "ASTNode"; }
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
