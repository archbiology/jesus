#include "weekday_rule.hpp"
#include "../../../../../ast/expr/literal_expr.hpp"

std::unique_ptr<Expr> WeekdayRule::parse(ParserContext &ctx)
{
    if (ctx.match(TokenType::LIGHDAY))
        return std::make_unique<LiteralExpr>(Value(1));

    if (ctx.match(TokenType::SKYDAY))
        return std::make_unique<LiteralExpr>(Value(2));

    if (ctx.match(TokenType::TREEDAY))
        return std::make_unique<LiteralExpr>(Value(3));

    if (ctx.match(TokenType::LAMPDAY))
        return std::make_unique<LiteralExpr>(Value(4));

    if (ctx.match(TokenType::FISHDAY))
        return std::make_unique<LiteralExpr>(Value(5));

    if (ctx.match(TokenType::WALKDAY))
        return std::make_unique<LiteralExpr>(Value(6));

    if (ctx.match(TokenType::SHABBAT))
        return std::make_unique<LiteralExpr>(Value(7));

    return nullptr;
}
