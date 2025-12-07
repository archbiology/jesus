#pragma once

#include "../expr/expr.hpp"
#include "stmt.hpp"
#include <string>
#include <memory>

REGISTER_FOR_UML(
    CreateVarStmt,
    .packageName("ast.stmt")
        .parentsList({"Stmt"})
        .fieldsList({"base_type", "name", "value"}));

/**
 * @brief Represents the creation of a new variable with a given value.
 *
 * "In the beginning God created..." — Genesis 1:1
 */
class CreateVarStmt : public Stmt
{
public:
    std::shared_ptr<CreationType> base_type;
    std::string name;
    std::unique_ptr<Expr> value;

    CreateVarStmt(std::shared_ptr<CreationType> type, std::string name, std::unique_ptr<Expr> value)
        : base_type(type), name(name), value(std::move(value)) {}

    /**
     * @brief Returns a string representation of the statement.
     *
     * "For nothing is hidden that will not be made manifest, nor is anything
     * secret that will not be known and come to light." — Luke 8:17
     */
    std::string toString() const override
    {

        std::string str = "CreateVarStmt(" + name;

        if (value)
            str += ", " + value->toString();

        str += ")";

        return str;
    }

    void accept(StmtVisitor &visitor) const override;
};
