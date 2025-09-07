#include "born_rule.hpp"
#include "../../../../../ast/expr/literal_expr.hpp"
#include "../../../../../types/known_types.hpp"

std::unique_ptr<Expr> BornRule::parse(ParserContext &ctx)
{
    if (ctx.match(TokenType::MALE))
        return std::make_unique<LiteralExpr>(Value(true), KnownTypes::SEX);

    if (ctx.match(TokenType::FEMALE))
        return std::make_unique<LiteralExpr>(Value(false), KnownTypes::SEX);

    return nullptr;
}
