#pragma once
#include "../../../grammar_rule.hpp"

class GiantsRule : public IGrammarRule
{
public:
    std::unique_ptr<Expr> parse(ParserContext &ctx) override;

    std::string toStr(GrammarRuleHashTable &visited) const override
    {
        return "Giants";
    }
};
