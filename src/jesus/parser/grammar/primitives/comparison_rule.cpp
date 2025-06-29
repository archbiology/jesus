#include "comparison_rule.hpp"

bool ComparisonRule::parse(ParserContext &ctx)
{
    if (!inner->parse(ctx))
        return false;

    while (ctx.matchAny({TokenType::GREATER,
                         TokenType::GREATER_EQUAL,
                         TokenType::LESS,
                         TokenType::LESS_EQUAL}))
    {
        if (!inner->parse(ctx))
            return false;
    }

    return true;
}

std::string ComparisonRule::toStr(GrammarRuleHashTable &visited) const
{
    if (visited.count(this))
        return "Comparison(...)";

    visited.insert(this);

    return "Comparison(" + inner->toStr(visited) + ")";
}
