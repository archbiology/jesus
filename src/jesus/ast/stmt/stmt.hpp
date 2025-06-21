#pragma once
#include "../ast_node.hpp"

/**
 * @brief Statements represent actions or commands (create variable, print, loops, etc).
 *
 */
class Stmt : public ASTNode
{
public:
    virtual ~Stmt() = default;
};
