#pragma once

#include "../expr/expr.hpp"
#include "stmt.hpp"
#include <string>
#include <memory>

/**
 * @brief Represents the update of an existing variable with a new value.
 *
 * "Be transformed by the renewing of your mind..." — Romans 12:2
 */
class UpdateVarStmt : public Stmt
{
public:
    std::string name;
    std::unique_ptr<Expr> value;

    UpdateVarStmt(const std::string name, std::unique_ptr<Expr> value)
        : name(name), value(std::move(value)) {}

    void accept(StmtVisitor &visitor) const override;
};
