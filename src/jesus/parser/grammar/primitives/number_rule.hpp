#pragma once
#include "../grammar_rule.hpp"

/**
 * @brief Matches a number literal.
 */
class NumberRule : public IGrammarRule
{
public:
    std::unique_ptr<Expr> parse(ParserContext &ctx) override;

    std::string toStr(GrammarRuleHashTable &visitedTable) const override
    {
        return "Number";
    }
};
