#pragma once
#include "grammar_rule.hpp"
#include <memory>

class GroupRule : public IGrammarRule
{
    std::shared_ptr<IGrammarRule> inner;

public:
    explicit GroupRule(std::shared_ptr<IGrammarRule> rule)
        : inner(std::move(rule)) {}

    std::unique_ptr<Expr> parse(ParserContext &ctx) override;

    std::string toStr(GrammarRuleHashTable &visitedTable) const override
    {
        if (visitedTable.count(this))
            return "Group(...)";

        visitedTable.insert(this);

        return "Group(" + inner->toStr(visitedTable) + ")";
    }
};
