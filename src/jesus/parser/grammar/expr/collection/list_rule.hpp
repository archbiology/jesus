#pragma once
#include "../../grammar_rule.hpp"

/**
 * @brief Grammar rule for parsing lists. E.g.: [70, 7]
 *
 * List → "[" ( Expression ( "," Expression )* )? "]"
 */
class ListRule : public IGrammarRule
{
private:
    std::shared_ptr<IGrammarRule> expression;

public:
    ListRule(std::shared_ptr<IGrammarRule> expr)
        : expression(expr) {}

    std::unique_ptr<Expr> parse(ParserContext &ctx) override;

    std::string toStr(GrammarRuleHashTable &visitedTable) const override
    {
        return "ListRule";
    }
};
