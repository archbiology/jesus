#include "string_rule.hpp"
#include "../../../../../ast/expr/literal_expr.hpp"
#include "../../../../../types/known_types.hpp"

std::unique_ptr<Expr> StringRule::parse(ParserContext &ctx)
{
    if (ctx.match(TokenType::RAW_STRING))
    {
        return std::make_unique<LiteralExpr>(ctx.previous().literal, KnownTypes::STRING);
    }

    return nullptr;
}
