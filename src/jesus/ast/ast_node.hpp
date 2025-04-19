#pragma once

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
     */
    virtual void execute() = 0;
};
