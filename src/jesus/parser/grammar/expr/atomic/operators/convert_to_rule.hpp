#pragma once

#include "../../../grammar_rule.hpp"

/**
 * @brief Parser for variable casting using 'to'
 *
 * son = parent to Son
 *
 * Similar to the following in other languages:
 *  son = (Son) parent
 */
class ConvertToRule : public IGrammarRule
{
private:
    std::shared_ptr<IGrammarRule> previousRule;

public:
    explicit ConvertToRule(std::shared_ptr<IGrammarRule> previousRule)
        : previousRule(std::move(previousRule)) {}

    std::unique_ptr<Expr> parse(ParserContext &ctx) override;

    std::string toStr(GrammarRuleHashTable &visited) const override
    {
        return "ConvertToRule()";
    }
};
