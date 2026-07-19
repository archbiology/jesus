#pragma once

#include "../expr/expr.hpp"
#include "stmt.hpp"
#include <string>
#include <memory>

REGISTER_FOR_UML(
    UpdateVarStmt,
    .packageName("ast.stmt")
        .parentsList({"Stmt"})
        .fieldsList({"name", "value"}));
/**
 * @brief Represents the update of an existing variable with a new value.
 *
 * "Be transformed by the renewing of your mind..." — Romans 12:2
 */
class UpdateVarStmt : public Stmt
{
public:
    std::string name;
    const VariableAddress address;
    std::unique_ptr<Expr> value;

    UpdateVarStmt(const std::string name, const VariableAddress address, std::unique_ptr<Expr> value)
        : name(name), address(address), value(std::move(value)) {}

    void accept(StmtVisitor &visitor) const override;

    std::string toString() const override
    {
        std::string str = "UpdateVarStmt('" + name + "', value: " + value->toString() + ")";
        return str;
    }
};
