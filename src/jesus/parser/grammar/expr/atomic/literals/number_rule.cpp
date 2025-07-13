#include "number_rule.hpp"
#include "../../../../../ast/expr/literal_expr.hpp"

std::unique_ptr<Expr> NumberRule::parse(ParserContext &ctx)
{
    if (ctx.matchAny({TokenType::INT, TokenType::DOUBLE}))
    {
        return std::make_unique<LiteralExpr>(ctx.previous().literal);
    }

    return nullptr;
}
