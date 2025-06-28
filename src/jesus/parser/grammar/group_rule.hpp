#pragma once
#include "grammar_rule.hpp"
#include <memory>

class GroupRule : public IGrammarRule
{
    std::shared_ptr<IGrammarRule> inner;

public:
    explicit GroupRule(std::shared_ptr<IGrammarRule> rule)
        : inner(std::move(rule)) {}

    bool parse(ParserContext &ctx) override;

    std::string toString() override
    {
        return "Group(" + inner->toString() + ")";
    }
};
