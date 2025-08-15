#pragma once
#include "stmt.hpp"

/**
 * @brief Represents an incomplete block stmt
 *
 * When someone is creating a class in the REPL, where the class body
 * can't be written in a single line, this statement tells the scanner to
 * keep consuming more tokens.
 */
class IncompleteBlockStmt : public Stmt
{
public:
    void accept(StmtVisitor &visitor) const override {};
    bool inProgress() const override { return true; }
};
