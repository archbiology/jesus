#pragma once

#include "../../../grammar_rule.hpp"
#include <memory>

/**
 * @brief Handles comparison operations: >, >=, <, <=
 *
 * "But let justice roll down like waters, and righteousness like an ever-flowing stream." — Amos 5:24
 */
class ComparisonRule : public IGrammarRule
{
private:
    std::shared_ptr<IGrammarRule> inner;

public:
    explicit ComparisonRule(std::shared_ptr<IGrammarRule> addition)
        : inner(std::move(addition)) {}

    std::unique_ptr<Expr> parse(ParserContext &ctx) override;

    std::string toStr(GrammarRuleHashTable &visited) const override;
};
