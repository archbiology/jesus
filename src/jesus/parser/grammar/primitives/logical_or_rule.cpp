#include "logical_or_rule.hpp"

bool LogicalOrRule::parse(ParserContext &ctx)
{
    if (!nextRule->parse(ctx))
        return false;

    while (ctx.match(TokenType::OR))
    {
        if (!nextRule->parse(ctx))
            return false;
    }
    return true;
}
