#pragma once
#include "../grammar_rule.hpp"

// CHAIN (sequence): rule1 >> rule2
class SequenceRule : public IGrammarRule
{
    std::shared_ptr<IGrammarRule> first;
    std::shared_ptr<IGrammarRule> second;

public:
    SequenceRule(std::shared_ptr<IGrammarRule> a, std::shared_ptr<IGrammarRule> b)
        : first(std::move(a)), second(std::move(b)) {}

    std::unique_ptr<Expr> parse(ParserContext &ctx) override
    {
        int backup = ctx.snapshot(); // Save current token position in case this rule fails

        auto firstExpr = first->parse(ctx);
        if (!firstExpr)
        {
            ctx.restore(backup); // Restore just the token position
            return nullptr;
        }

        auto secondExpr = second->parse(ctx);
        if (!secondExpr)
        {
            ctx.restore(backup); // Restore just the token position
            return nullptr;
        }

        return secondExpr;
    }

    std::string toStr(GrammarRuleHashTable &visitedTable) const override
    {
        if (visitedTable.count(this))
            return "Sequence(...)";

        visitedTable.insert(this);

        return "Sequence( " + first->toStr(visitedTable) + " >> " + second->toStr(visitedTable) + ")";
    }
};
