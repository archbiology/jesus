#pragma once
#include "../grammar_rule.hpp"

/**
 * @brief Parses expressions combined by logical OR (e.g., a or b or c).
 *
 * Logical OR sits at the top of the logical precedence hierarchy.
 *
 * Example:
 *   1 or 2 or 3
 *
 * "For where two or three gather in my name, there am I with them." — Matthew 18:20
 */
class LogicalOrRule : public IGrammarRule
{
private:
    std::shared_ptr<IGrammarRule> nextRule;

public:
    explicit LogicalOrRule(std::shared_ptr<IGrammarRule> next) : nextRule(std::move(next)) {}

    std::unique_ptr<Expr> parse(ParserContext &ctx) override;

    std::string toStr(GrammarRuleHashTable &visitedTable) const override
    {
        if (visitedTable.count(this))
            return "LogicalOr(...)";

        visitedTable.insert(this);

        return "LogicalOr(" + nextRule->toStr(visitedTable) + ")";
    }
};
