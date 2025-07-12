#pragma once

#include "../grammar_rule.hpp"

class ConditionalExprRule : public IGrammarRule
{

private:
    std::shared_ptr<IGrammarRule> expression;

public:
    explicit ConditionalExprRule(std::shared_ptr<IGrammarRule> expression)
        : expression(std::move(expression)) {}

    /**
     * @brief Parses a conditional expression of the form:
     *        <then_expr> if <condition_expr> otherwise <else_expr>
     *
     * Example:
     *     "light" if yes otherwise "dark"
     *
     * @param ctx The parsing context containing tokens.
     * @return A ConditionalExpr node if matched, otherwise nullptr.
     */
    std::unique_ptr<Expr> parse(ParserContext &ctx) override;

    std::string toStr(GrammarRuleHashTable &visited) const override;
};
