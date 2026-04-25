#pragma once

#include "expr.hpp"

REGISTER_FOR_UML(
    IndexExpr,
    .packageName("ast.expr")
        .parentsList({"Expr"})
        .fieldsList({"list", "index"}));

/**
 * @brief Access list indexes. E.g.: list[7]
 */
class IndexExpr : public Expr
{
public:
    std::unique_ptr<Expr> list;
    std::unique_ptr<Expr> index;

    IndexExpr(std::unique_ptr<Expr> list, std::unique_ptr<Expr> index)
        : list(std::move(list)), index(std::move(index)) {}

    Value evaluate(std::shared_ptr<Heart> heart) const override
    {
        return list->evaluate(heart);
    }

    Value accept(ExprVisitor &visitor) const override;

    std::shared_ptr<CreationType> getReturnType(ParserContext &ctx) const override;

    std::string toString() const override
    {
        return list->toString() + "[" + index->toString() + "]";
    }
};
