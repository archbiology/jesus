#pragma once

#include "assignable_expr.hpp"

REGISTER_FOR_UML(
    GetAttributeExpr,
    .packageName("ast.expr")
        .parentsList({"Expr"})
        .fieldsList({"object", "attribute"}));

class GetAttributeExpr : public AssignableExpr
{
public:
    std::unique_ptr<Expr> object;
    std::string attribute;

    GetAttributeExpr(std::unique_ptr<Expr> object, std::string attribute)
        : object(std::move(object)), attribute(std::move(attribute)), AssignableExpr(ExprKind::GetAttribute) {}

    Value evaluate(std::shared_ptr<Heart> heart) const override
    {
        return object->evaluate(heart);
    }

    Value accept(ExprVisitor &visitor) const override;

    void assign(Interpreter &interpreter, const Value &value) const override;

    /**
     * @brief Get the return type of the expression, so that variable
     *  creation and update can be enforced at parse time.
     *
     * "Flesh gives birth to flesh, but the Spirit gives birth to spirit." — John 3:6
     */
    std::shared_ptr<CreationType> getReturnType(ParserContext &ctx) const override;
};
