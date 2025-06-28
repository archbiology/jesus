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

    bool parse(ParserContext &ctx) override
    {
        ParserContext backup = ctx.snapshot();
        if (first->parse(ctx))
        {
            if (second->parse(ctx))
                return true;
        }
        ctx = backup;
        return false;
    }

    std::string toStr(GrammarRuleHashTable &visitedTable) const override
    {
        if (visitedTable.count(this))
            return "Sequence(...)";

        visitedTable.insert(this);

        return "Sequence( " + first->toStr(visitedTable) + " >> " + second->toStr(visitedTable) + ")";
    }
};
