
#pragma once
#include "../../../grammar_rule.hpp"

/**
 * @brief Matches a string literal with single quotes: 'Jesus loves you'.
 */
class StringRule : public IGrammarRule
{
public:
    std::unique_ptr<Expr> parse(ParserContext &ctx) override;

    std::string toStr(GrammarRuleHashTable &visitedTable) const override
    {
        return "String";
    }
};
