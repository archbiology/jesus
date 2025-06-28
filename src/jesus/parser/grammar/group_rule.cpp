#include "group_rule.hpp"
#include "../../lexer/token_type.hpp"

bool GroupRule::parse(ParserContext &ctx)
{
    if (!ctx.match(TokenType::LEFT_PAREN))
        return false;

    if (!inner->parse(ctx))
        return false;

    if (!ctx.match(TokenType::RIGHT_PAREN))
        return false;

    return true;
}
