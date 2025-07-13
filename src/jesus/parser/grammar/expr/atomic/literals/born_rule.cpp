#include "born_rule.hpp"
#include "../../../../../ast/expr/literal_expr.hpp"

std::unique_ptr<Expr> BornRule::parse(ParserContext &ctx)
{
    if (ctx.match(TokenType::MALE))
        return std::make_unique<LiteralExpr>(Value(true));

    if (ctx.match(TokenType::FEMALE))
        return std::make_unique<LiteralExpr>(Value(false));

    return nullptr;
}
