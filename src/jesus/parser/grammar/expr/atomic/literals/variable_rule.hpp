#pragma once
#include "../../../grammar_rule.hpp"

/**
 * @brief Grammar rule that matches variable references.
 *
 * It corresponds to a VariableExpr node in the AST.
 *
 * “The LORD called Moses by name from the tent of meeting and said…”
 * — Leviticus 1:1
 */
class VariableRule : public IGrammarRule
{
public:
    std::unique_ptr<Expr> parse(ParserContext &ctx) override;

    std::string toStr(GrammarRuleHashTable &visited) const override
    {
        return "Variable";
    }
};
