#include "number_rule.hpp"
#include "../../../../../ast/expr/literal_expr.hpp"
#include "../../../../../types/known_types.hpp"

std::unique_ptr<Expr> NumberRule::parse(ParserContext &ctx)
{
    if (ctx.match(TokenType::INT))
        return std::make_unique<LiteralExpr>(ctx.previous().literal, KnownTypes::INT);

    if (ctx.match(TokenType::DOUBLE))
        return std::make_unique<LiteralExpr>(ctx.previous().literal, KnownTypes::DOUBLE);

    return nullptr;
}
