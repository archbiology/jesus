#pragma once
#include "../grammar_rule.hpp"

/**
 * @brief Matches a number literal.
 */
class NumberRule : public IGrammarRule
{
public:
    bool parse(ParserContext &ctx) override;

    std::string toString() override
    {
        return "Number";
    }
};
