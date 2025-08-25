#pragma once

#include "expr.hpp"

class GetAttributeExpr : public Expr
{
public:
    std::unique_ptr<Expr> object;
    std::string attribute;

    GetAttributeExpr(std::unique_ptr<Expr> object, std::string attribute)
        : object(std::move(object)), attribute(std::move(attribute)) {}

    Value evaluate(Heart *heart) const override
    {
        return object->evaluate(heart);
    }

    Value accept(ExprVisitor &visitor) const override;
};
