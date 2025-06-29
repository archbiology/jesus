#include "versus_rule.hpp"

VersusRule::VersusRule(std::shared_ptr<IGrammarRule> lhs, std::shared_ptr<IGrammarRule> rhs)
    : lhsRule(std::move(lhs)), rhsRule(std::move(rhs))
{
}

bool VersusRule::parse(ParserContext &ctx)
{
    // Parse left-hand side expression
    if (!lhsRule->parse(ctx))
        return false;

    // Check if next token is VERSUS
    if (!ctx.match(TokenType::VERSUS))
        return true; // no operator found, expression is just lhs

    // Parse right-hand side expression
    return rhsRule->parse(ctx);
}

std::string VersusRule::toStr(GrammarRuleHashTable &visitedTable) const
{
    if (visitedTable.count(this))
        return "Versus(...)";

    visitedTable.insert(this);

    return "Versus(" + lhsRule->toStr(visitedTable) + " vs " + rhsRule->toStr(visitedTable) + ")";
}
