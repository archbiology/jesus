#pragma once

#include "../expr/expr.hpp"
#include "../../types/creation_type.hpp"
#include "stmt.hpp"
#include <string>
#include <memory>

/**
 * @brief Represents the creation of a new variable using 'ask'
 *
 * E.g.: create int age = ask "What is your age?"
 *
 * "Ask and it will be given to you; seek and you will find; knock and the door will be opened to you." — Matthew 7:7
 */
class CreateVarWithAskStmt : public Stmt
{
public:
    const std::shared_ptr<CreationType> var_type;
    std::string var_name;
    std::shared_ptr<Expr> ask_expr; // Type is AskExpr

    CreateVarWithAskStmt(const std::shared_ptr<CreationType> type, std::string name, std::unique_ptr<Expr> ask_expr)
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

        std::string str = "CreateVarWithAsk(" + var_name + " = " + ask_expr->toString() + ")";

        return str;
    }
};
