
#pragma once
#include "../../../grammar_rule.hpp"

/**
 * @brief Matches a formatted string literal: "Hi, {user}".
 *
 * A formatted string is like a raw string, but may include {varname} placeholders.
 * Only variables are allowed inside braces (no full expressions).
 *
 * Formatted strings also use double quotes, '"', whereas raw strings use single quotes.
 */
class FormattedStringRule : public IGrammarRule
{
public:
    std::unique_ptr<Expr> parse(ParserContext &ctx) override;

    std::string toStr(GrammarRuleHashTable &visitedTable) const override
    {
        return "FormattedStringRule";
    }
};
