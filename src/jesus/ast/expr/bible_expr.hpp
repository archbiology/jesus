#pragma once

#include "expr.hpp"

REGISTER_FOR_UML(
    BibleExpr,
    .packageName("ast.expr")
        .parentsList({"Expr"})
        .fieldsList({"reference"}));

/**
 * @brief create text verse = bible 'john 3:16'
 */
class BibleExpr : public Expr
{
public:
    std::unique_ptr<Expr> reference;
    BibleExpr(std::unique_ptr<Expr> ref) : reference(std::move(ref)) {}

    Value evaluate(std::shared_ptr<Heart> heart) const override
    {
        return reference->evaluate(heart);
    }

    Value accept(ExprVisitor &visitor) const override;

    std::shared_ptr<CreationType> getReturnType(ParserContext &ctx) const override;

    virtual std::string toString() const override { return "BibleExpr(" + reference->toString() + ")"; }
};
