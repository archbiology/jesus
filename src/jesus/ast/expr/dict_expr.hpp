#pragma once

#include "expr.hpp"

REGISTER_FOR_UML(
    DictExpr,
    .packageName("ast.expr")
        .parentsList({"Expr"})
        .fieldsList({"elements"}));

/**
 * @brief Represents a dict. E.g.: {"name": "Jesus", "title": "Christ", "role": "Savior"}
 */
class DictExpr : public Expr
{
public:
    std::vector<std::pair<std::unique_ptr<Expr>, std::unique_ptr<Expr>>> entries;
    std::shared_ptr<CreationType> dictType;

    DictExpr(std::vector<std::pair<std::unique_ptr<Expr>, std::unique_ptr<Expr>>> entries,
             std::shared_ptr<CreationType> dictType)
        : entries(std::move(entries)), dictType(dictType) {}

    Value evaluate(std::shared_ptr<Heart> scope) const override
    {
        throw std::runtime_error("Should call Interpreter::visitDictExpr");
    }

    Value accept(ExprVisitor &visitor) const override;

    std::shared_ptr<CreationType> getReturnType(ParserContext &ctx) const override
    {
        return dictType;
    }
};
