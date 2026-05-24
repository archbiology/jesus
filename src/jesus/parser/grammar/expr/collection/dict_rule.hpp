#pragma once
#include "../../grammar_rule.hpp"

/**
 * @brief Grammar rule for parsing dict. E.g.: {"name": "Jesus Christ"}
 */
class DictRule : public IGrammarRule
{
private:
    std::shared_ptr<IGrammarRule> expression;

public:
    DictRule(std::shared_ptr<IGrammarRule> expr) : expression(expr) {}

    std::unique_ptr<Expr> parse(ParserContext &ctx) override;

    std::string toStr(GrammarRuleHashTable &visitedTable) const override
    {
        return "DictRule";
    }
};
