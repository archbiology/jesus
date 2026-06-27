#pragma once

#include "expr.hpp"

REGISTER_FOR_UML(
    CreateInstanceExpr,
    .packageName("ast.expr")
        .parentsList({"Expr"})
        .fieldsList({"name", "klass", "constructorArgs"}));

class CreateInstanceExpr : public Expr
{
public:
    const std::string name;
    const std::shared_ptr<CreationType> klass;
    std::unique_ptr<Expr> constructorArgs;

public:
    explicit CreateInstanceExpr(const std::string name, std::shared_ptr<CreationType> klass, std::unique_ptr<Expr> constructorArgs = nullptr)
        : name(std::move(name)), klass(std::move(klass)), constructorArgs(std::move(constructorArgs)) {}

    Value evaluate(std::shared_ptr<Heart> scope) const override
    {
        throw std::runtime_error("CreateInstanceExpr cannot be directly evaluated without a Visitor.");
    }

    Value accept(ExprVisitor &visitor) const override;

    /**
     * @brief Get the return type of the expression, so that variable
     *  creation and update can be enforced at parse time.
     *
     * "Flesh gives birth to flesh, but the Spirit gives birth to spirit." — John 3:6
     */
    std::shared_ptr<CreationType> getReturnType(ParserContext &ctx) const override
    {
        return klass;
    }
};
