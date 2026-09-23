#pragma once

#include "expr.hpp"
#include "../../lexer/token_type.hpp"
#include <string>
#include <vector>

REGISTER_FOR_UML(FormatExpr, .packageName("ast.expr").parentsList({"Expr"}).fieldsList({"formatters", "inner"}));

/**
 * @brief Represents a styled/colored text expression.
 *
 * Applies ANSI formatting (color and style) to a nested expression, e.g.:
 *
 *     say bold red 'Jesus saves'
 *     say red('warning')
 *
 * "I am the light of the world. Whoever follows me will not walk in darkness,
 *  but will have the light of life." — John 8:12
 */
class FormatExpr : public Expr
{
public:
    std::vector<TokenType> formatters;
    std::unique_ptr<Expr> inner;

    FormatExpr(std::vector<TokenType> formatters, std::unique_ptr<Expr> inner)
        : formatters(std::move(formatters)), inner(std::move(inner)), Expr(ExprKind::Other)
    {
    }

    Value evaluate(std::shared_ptr<Heart> heart) const override;

    Value accept(ExprVisitor &visitor) const override;

    std::shared_ptr<CreationType> getReturnType(ParserContext &ctx) const override;

    bool canBeUsedInFormattedString() const override
    {
        return true;
    }

    std::string toString() const override;
};
