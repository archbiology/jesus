#pragma once

#include "../grammar_rule.hpp"

// OR: rule1 | rule2
class OrRule : public IGrammarRule
{
    std::vector<std::shared_ptr<IGrammarRule>> rules;

public:
    OrRule(std::shared_ptr<IGrammarRule> a, std::shared_ptr<IGrammarRule> b)
    {
        rules.push_back(a);
        rules.push_back(b);
    }

    OrRule &operator|(std::shared_ptr<IGrammarRule> r)
    {
        rules.push_back(r);
        return *this;
    }

    bool parse(ParserContext &ctx) override
    {
        auto count = 1;
        for (auto &rule : rules)
        {
            ParserContext backup = ctx.snapshot();
            if (rule->parse(ctx))
                return true;

            ctx = backup;
            count++;
        }
        return false;
    }

    std::string toStr(GrammarRuleHashTable &visitedTable) const override
    {
        if (visitedTable.count(this))
            return "Or(...)";

        visitedTable.insert(this);

        std::string str = "";
        bool first = true;
        for (const auto &rule : rules)
        {
            if (!first)
                str += " | ";

            str += rule->toStr(visitedTable);

            first = false;
        }

        return str;
    }
};
