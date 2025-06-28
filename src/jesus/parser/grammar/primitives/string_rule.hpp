
#pragma once
#include "../grammar_rule.hpp"

/**
 * @brief Matches a string literal.
 */
class StringRule : public IGrammarRule
{
public:
    bool parse(ParserContext &ctx) override;

    std::string toString() override
    {
        return "String";
    }
};
