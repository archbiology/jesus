#pragma once

#include "grammar_rule.hpp"
#include "../parser_context.hpp"
#include <memory>

/**
 * @brief Rule for parsing unary expressions, such as "not expr" or "-expr".
 *
 * This rule supports unary operators like 'not', '+', and '-'.
 * It defers to an inner rule (e.g., Primary or Expression) for parsing the operand.
 *
 * "Put on the full armor of God, so that you can take your stand against the devil’s schemes."
 * — Ephesians 6:11
 */
class UnaryRule : public IGrammarRule
{
private:
    std::shared_ptr<IGrammarRule> operandRule;

public:
    explicit UnaryRule(std::shared_ptr<IGrammarRule> operandRule)
        : operandRule(std::move(operandRule)) {}

    bool parse(ParserContext &ctx) override;

    std::string toStr(GrammarRuleHashTable &visitedTable) const override;
};
