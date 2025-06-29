#pragma once

#include "../grammar_rule.hpp"

/**
 * @brief Grammar rule to parse equality comparisons (==, !=).
 *
 * This rule matches expressions involving equality or inequality checks,
 * such as `a == b` or `x != y`. It relies on a lower-precedence operand rule,
 * typically a ComparisonRule, and parses left-associative binary expressions.
 *
 * Example matches:
 * - 1 == 2
 * - value != (3 + 5)
 *
 * "The LORD detests differing weights, but he delights in accurate scales."
 * — Proverbs 11:1
 */
class EqualityRule : public IGrammarRule
{
private:
    std::shared_ptr<IGrammarRule> operand;

public:
    /**
     * @brief Constructs the rule with its lower-precedence operand (e.g., Comparison).
     *
     * @param operandRule A shared pointer to the rule that parses the operand expressions.
     */
    explicit EqualityRule(std::shared_ptr<IGrammarRule> operandRule)
        : operand(std::move(operandRule)) {}

    bool parse(ParserContext &ctx) override;

    std::string toStr(GrammarRuleHashTable &visited) const override;
};
