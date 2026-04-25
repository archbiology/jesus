#pragma once

#include "expr.hpp"

REGISTER_FOR_UML(
    ListExpr,
    .packageName("ast.expr")
        .parentsList({"Expr"})
        .fieldsList({"elements"}));

/**
 * @brief Represents a list. E.g.: [1, 2, 3]
 */
class ListExpr : public Expr
{
public:
    std::vector<std::unique_ptr<Expr>> elements;

    ListExpr(std::vector<std::unique_ptr<Expr>> elements)
        : elements(std::move(elements)) {}

    Value evaluate(std::shared_ptr<Heart> scope) const override
    {
        throw std::runtime_error("Should call Interpreter::evalListExpr through Interpreter::visitListEpxr");
    }

    Value accept(ExprVisitor &visitor) const override;

    std::shared_ptr<CreationType> getReturnType(ParserContext &ctx) const override;
};
