#pragma once

#include "../expr/expr.hpp"
#include "stmt.hpp"
#include <string>
#include <memory>

/**
 * @brief Represents the creation of a new variable with a given value.
 *
 * "In the beginning God created..." — Genesis 1:1
 */
class CreateVarStmt : public Stmt
{
public:
    std::string name;
    std::unique_ptr<Expr> value;

    CreateVarStmt(std::string name, std::unique_ptr<Expr> value)
        : name(name), value(std::move(value)) {}
};
