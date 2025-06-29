#include "equality_rule.hpp"
#include "../../../lexer/token_type.hpp"

bool EqualityRule::parse(ParserContext &ctx)
{
    if (!operand->parse(ctx))
        return false;

    while (ctx.matchAny({TokenType::EQUAL_EQUAL, TokenType::NOT_EQUAL}))
    {
        if (!operand->parse(ctx))
            return false;
    }

    return true;
}

std::string EqualityRule::toStr(GrammarRuleHashTable &visited) const
{
    if (visited.count(this))
        return "Equality(...)";

    visited.insert(this);

    return "Equality(" + operand->toStr(visited) + ")";
}
