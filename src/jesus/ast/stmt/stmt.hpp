#pragma once
#include "../ast_node.hpp"

class StmtVisitor; // Forward declaration

/**
 * @brief Statements represent actions or commands (create variable, print, loops, etc).
 *
 */
class Stmt : public ASTNode
{
public:
    virtual ~Stmt() = default;

    virtual void accept(StmtVisitor &visitor) const = 0;
};
