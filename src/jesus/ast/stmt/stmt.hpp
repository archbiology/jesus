#pragma once
#include "../ast_node.hpp"

class StmtVisitor; // Forward declaration

REGISTER_FOR_UML(
    Stmt,
    .parentsList({"ASTNode"})
        .packageName("ast.stmt")
        .methodsList({
            "accept",
            "inProgress",
        }));

/**
 * @brief Statements represent actions or commands (create variable, print, loops, etc).
 *
 */
class Stmt : public ASTNode
{
public:
    virtual ~Stmt() = default;

    virtual void accept(StmtVisitor &visitor) const = 0;

    /**
     * @brief Tells if more tokens are expected.
     *
     * Useful when someone is creating a class in the REPL, where the class body
     * can't be written in a single line. This tells the scanner to keep
     * scanning tokens before executing the AST.
     */
    virtual bool inProgress() const { return false; }
};
