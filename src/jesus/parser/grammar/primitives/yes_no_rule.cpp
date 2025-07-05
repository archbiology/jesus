#include "yes_no_rule.hpp"
#include "../../../ast/expr/literal_expr.hpp"

std::unique_ptr<Expr> YesNoRule::parse(ParserContext &ctx)
{
    if (ctx.match(TokenType::YES))
        return std::make_unique<LiteralExpr>(Value(true));

    if (ctx.match(TokenType::NO))
        return std::make_unique<LiteralExpr>(Value(false));

    return nullptr;
}