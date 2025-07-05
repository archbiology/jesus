#pragma once
#include "../grammar_rule.hpp"

/**
 * @brief Parses boolean literals: `yes` or `no`.
 *
 * Known as Boolean `true` or `false` in other languages.
 *
 * “Let your ‘Yes’ be ‘Yes,’ and your ‘No,’ ‘No’; anything beyond this comes from the evil one.”
 * — Matthew 5:37
 */
class YesNoRule : public IGrammarRule
{
public:
    bool parse(ParserContext &ctx) override
    {
        return ctx.match(TokenType::YES) || ctx.match(TokenType::NO);
    }

    std::string toStr(GrammarRuleHashTable &) const override
    {
        return "YesNo";
    }
};
