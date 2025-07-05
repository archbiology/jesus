#pragma once

#include "../grammar_rule.hpp"
#include <memory>

/**
 * @brief Rule to parse the 'VERSUS' binary operator expressions, also known as XOR.
 *
 * This rule parses expressions with the 'VERSUS' operator, e.g. `expr VERSUS expr`.
 *
 * "No one can serve two masters. Either you will hate the one and love the other,
 * or you will be devoted to the one and despise the other.
 * You cannot serve both God and money." — Matthew 6:24
 */
class VersusRule : public IGrammarRule
{
private:
    std::shared_ptr<IGrammarRule> lhsRule;
    std::shared_ptr<IGrammarRule> rhsRule;

public:
    explicit VersusRule(std::shared_ptr<IGrammarRule> lhs, std::shared_ptr<IGrammarRule> rhs);

    std::unique_ptr<Expr> parse(ParserContext &ctx) override;

    std::string toStr(GrammarRuleHashTable &visitedTable) const override;
};
