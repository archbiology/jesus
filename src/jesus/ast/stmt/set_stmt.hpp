#pragma once

#include "stmt.hpp"

#include "../expr/expr.hpp"
#include <string>
#include <memory>

/**
 *  Represents "set variable to value"
 */
class SetStmt : public Stmt
{
public:
    std::string name;
    std::unique_ptr<Expr> value;

    SetStmt(const std::string &name, std::unique_ptr<Expr> value)
        : name(name), value(std::move(value)) {}
};
