#pragma once

#include "../expr/expr.hpp"
#include "../../types/creation_type.hpp"
#include "stmt.hpp"
#include <string>
#include <memory>

REGISTER_FOR_UML(
    UpdateVarWithAskStmt,
    .packageName("ast.stmt")
        .parentsList({"Stmt"})
        .fieldsList({"type", "name", "ask_expr"}));
/**
 * @brief Represents the update of an existing variable using 'ask'.
 *
 * "Be transformed by the renewing of your mind..." — Romans 12:2
 */
class UpdateVarWithAskStmt : public Stmt
{
public:
    const std::shared_ptr<CreationType> var_type;
    const std::string var_name;
    std::shared_ptr<Expr> ask_expr; // Type is AskExpr

    UpdateVarWithAskStmt(const std::shared_ptr<CreationType> type, std::string name, std::unique_ptr<Expr> ask_expr)
        : var_type(type), var_name(name), ask_expr(std::move(ask_expr)) {}

    void accept(StmtVisitor &visitor) const override;

    /**
     * @brief Returns a string representation of the statement.
     *
     * "For nothing is hidden that will not be made manifest, nor is anything
     * secret that will not be known and come to light." — Luke 8:17
     */
    std::string toString() const override
    {

        std::string str = "UpdateVarWithAskStmt(" + var_name + " = " + ask_expr->toString() + ")";

        return str;
    }
};
