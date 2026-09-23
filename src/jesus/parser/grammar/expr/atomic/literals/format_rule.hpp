#pragma once

#include "../../../grammar_rule.hpp"
#include "../../../../../lexer/token_type.hpp"

/**
 * @brief Matches ANSI format modifier expressions: bold, italic, red, green, blue, yellow.
 *
 *     say bold red 'Jesus'
 *     say yellow 'warning'
 *     say red('error')
 */
class FormatRule : public IGrammarRule
{
private:
    std::shared_ptr<IGrammarRule> valueRule;

public:
    explicit FormatRule(std::shared_ptr<IGrammarRule> operand) : valueRule(std::move(operand))
    {
    }

    std::unique_ptr<Expr> parse(ParserContext &ctx) override;

    std::string toStr(GrammarRuleHashTable &visitedTable) const override
    {
        return "Format(" + valueRule->toStr(visitedTable) + ")";
    }
};
