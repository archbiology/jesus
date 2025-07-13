#pragma once

#include "../../../grammar_rule.hpp"

/**
 * @brief Parses biological/DNA sex literals: `male` or `female`.
 *
 * "So God created mankind in his own image, in the image of God he created them; male and female he created them."
 * — Genesis 1:27
 */
class BornRule : public IGrammarRule
{
public:
    std::unique_ptr<Expr> parse(ParserContext &ctx) override;

    std::string toStr(GrammarRuleHashTable &) const override
    {
        return "Born";
    }
};
