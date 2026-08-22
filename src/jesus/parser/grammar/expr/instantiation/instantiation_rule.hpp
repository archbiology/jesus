#pragma once
#include "../../grammar_rule.hpp"

/**
 * @brief Grammar rule for class instantiation calls. E.g.: Person()
 *
 * Instantiation → IDENTIFIER "(" ( Expression ( "," Expression )* )? ")"
 *
 * Only class names may be instantiated this way; any other identifier
 * followed by '(' is reported as an invalid class instantiation.
 */
class InstantiationRule : public IGrammarRule
{
  private:
    std::shared_ptr<IGrammarRule> expression;

  public:
    InstantiationRule(std::shared_ptr<IGrammarRule> expr) : expression(expr) {}

    std::unique_ptr<Expr> parse(ParserContext &ctx) override;

    std::string toStr(GrammarRuleHashTable &visitedTable) const override { return "CreateInstanceRule"; }
};
