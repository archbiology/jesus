#pragma once
#include "../../../grammar_rule.hpp"
#include "../../../../parser_context.hpp"
#include <memory>

/**
 * @brief Handles addition and subtraction expressions (e.g., a + b, a - b).
 *
 * This rule parses expressions with + or - operators by left-associative chaining of
 * `MultiplicationRule`s. For example, it parses `a + b - c` as ((a + b) - c).
 *
 * “The blessing of the Lord makes rich, and he adds no sorrow with it.”
 * — Proverbs 10:22
 */
class AdditionRule : public IGrammarRule
{
private:
    std::shared_ptr<IGrammarRule> inner;

public:
    explicit AdditionRule(std::shared_ptr<IGrammarRule> innerRule)
        : inner(std::move(innerRule)) {}

    std::unique_ptr<Expr> parse(ParserContext &ctx) override;

    std::string toStr(GrammarRuleHashTable &visited) const override
    {
        if (visited.count(this))
            return "Addition(...)";

        visited.insert(this);

        return "Addition(" + inner->toStr(visited) + ")";
    }
};
