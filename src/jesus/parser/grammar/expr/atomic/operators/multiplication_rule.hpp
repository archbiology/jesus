#pragma once

#include "../../../grammar_rule.hpp"

class MultiplicationRule : public IGrammarRule
{
private:
    std::shared_ptr<IGrammarRule> unary;

public:
    explicit MultiplicationRule(std::shared_ptr<IGrammarRule> unaryRule)
        : unary(std::move(unaryRule)) {}

    std::unique_ptr<Expr> parse(ParserContext &ctx) override;

    std::string toStr(GrammarRuleHashTable &visited) const override;
};
