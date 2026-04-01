#pragma once
#include "../../grammar_rule.hpp"

class BibleExprRule : public IGrammarRule
{
    std::shared_ptr<IGrammarRule> expression;

public:
    explicit BibleExprRule(std::shared_ptr<IGrammarRule> expr) : expression(std::move(expr)) {}

    std::unique_ptr<Expr> parse(ParserContext &ctx) override;

    std::string toStr(GrammarRuleHashTable &visited) const override
    {
        return "BibleExpr";
    }
};
