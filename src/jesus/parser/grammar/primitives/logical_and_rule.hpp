#pragma once

#include "../grammar_rule.hpp"

/**
 * @brief Parses logical AND expressions (e.g. a AND b AND c).
 *
 * "Two are better than one, because they have a good return for their labor."
 * — Ecclesiastes 4:9
 */
class LogicalAndRule : public IGrammarRule
{
private:
    std::shared_ptr<IGrammarRule> nextRule;

public:
    explicit LogicalAndRule(std::shared_ptr<IGrammarRule> next)
        : nextRule(std::move(next)) {}

    std::unique_ptr<Expr> parse(ParserContext &ctx) override;

    std::string toStr(GrammarRuleHashTable &visited) const override;
};
