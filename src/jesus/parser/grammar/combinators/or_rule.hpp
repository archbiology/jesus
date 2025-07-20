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

    std::unique_ptr<Expr> parse(ParserContext &ctx) override
    {
        for (auto &rule : rules)
        {
            int backup = ctx.snapshot(); // Save current token position in case this rule fails
            auto result = rule->parse(ctx);        // Try this rule

            if (result) // If it parsed successfully
                return result;

            ctx.restore(backup); // Restore just the token position and try next
        }

        return nullptr; // None of the rules matched
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
