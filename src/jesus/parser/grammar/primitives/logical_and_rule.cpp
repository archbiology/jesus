#include "logical_and_rule.hpp"

bool LogicalAndRule::parse(ParserContext &ctx)
{
    if (!nextRule->parse(ctx))
        return false;

    while (ctx.match(TokenType::AND))
    {
        if (!nextRule->parse(ctx))
            return false;
    }
    return true;
}

std::string LogicalAndRule::toStr(GrammarRuleHashTable &visited) const
{
    if (visited.count(this))
        return "LogicalAnd(...)";

    visited.insert(this);

    return "LogicalAnd(" + nextRule->toStr(visited) + ")";
}
