#include "string_rule.hpp"
#include "../../../ast/expr/literal_expr.hpp"

std::unique_ptr<Expr> StringRule::parse(ParserContext &ctx)
{
    if (ctx.match(TokenType::STRING))
    {
        return std::make_unique<LiteralExpr>(ctx.previous().literal);
    }

    return nullptr;
}
