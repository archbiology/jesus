#include "yes_no_rule.hpp"
#include "../../../../../ast/expr/literal_expr.hpp"
#include "../../../../../types/known_types.hpp"

std::unique_ptr<Expr> YesNoRule::parse(ParserContext &ctx)
{
    if (ctx.match(TokenType::YES))
        return std::make_unique<LiteralExpr>(Value(true), KnownTypes::BOOLEAN);

    if (ctx.match(TokenType::NO))
        return std::make_unique<LiteralExpr>(Value(false), KnownTypes::BOOLEAN);

    return nullptr;
}