#include "multiplication_rule.hpp"

bool MultiplicationRule::parse(ParserContext &ctx)
{
    if (!unary->parse(ctx))
        return false;

    while (ctx.matchAny({TokenType::STAR, TokenType::SLASH}))
    {
        if (!unary->parse(ctx))
            return false;
    }

    return true;
}

std::string MultiplicationRule::toStr(GrammarRuleHashTable &visited) const
{
    if (visited.count(this))
        return "Multiplication(...)";

    visited.insert(this);

    return "Multiplication(" + unary->toStr(visited) + ")";
}
