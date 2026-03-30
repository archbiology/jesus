#pragma once
#include "../../../grammar_rule.hpp"

class ConfessRule : public IGrammarRule
{
public:
    std::unique_ptr<Expr> parse(ParserContext &ctx) override;

    std::string toStr(GrammarRuleHashTable &visited) const override
    {
        return "Confess";
    }
};
