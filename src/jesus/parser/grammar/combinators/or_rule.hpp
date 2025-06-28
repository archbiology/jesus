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

    std::string toString() override
    {
        std::string str = "OrRule(";
        bool firstGone = false;
        for (auto &rule : rules)
        {
            if (firstGone)
            {
                str += "|";
            }

            str += rule->toString();

            firstGone = true;
        }
        str += ")";

        return str;
    }
};
