#include "weekday_rule.hpp"
#include "../../../../../ast/expr/literal_expr.hpp"
#include "../../../../../types/known_types.hpp"

std::unique_ptr<Expr> WeekdayRule::parse(ParserContext &ctx)
{
    if (ctx.match(TokenType::LIGHDAY))
        return std::make_unique<LiteralExpr>(Value(1), KnownTypes::WEEKDAY);

    if (ctx.match(TokenType::SKYDAY))
        return std::make_unique<LiteralExpr>(Value(2), KnownTypes::WEEKDAY);

    if (ctx.match(TokenType::TREEDAY))
        return std::make_unique<LiteralExpr>(Value(3), KnownTypes::WEEKDAY);

    if (ctx.match(TokenType::LAMPDAY))
        return std::make_unique<LiteralExpr>(Value(4), KnownTypes::WEEKDAY);

    if (ctx.match(TokenType::FISHDAY))
        return std::make_unique<LiteralExpr>(Value(5), KnownTypes::WEEKDAY);

    if (ctx.match(TokenType::WALKDAY))
        return std::make_unique<LiteralExpr>(Value(6), KnownTypes::WEEKDAY);

    if (ctx.match(TokenType::SHABBAT))
        return std::make_unique<LiteralExpr>(Value(7), KnownTypes::WEEKDAY);

    return nullptr;
}
