#pragma once
#include "../../../grammar_rule.hpp"

/**
 * @brief Grammar rule for accessing the current instance: "I" or "my".
 *
 * "I" and "my" refer to the object on which the current method is executing.
 * Internally, both are resolved through the hidden "$self" variable.
 *
 * Postfix rules then use this instance to resolve expressions such as:
 *   "my name"       → access the current object's "name" attribute
 *   "I like 'water'" → call the "like" method on the current object
 */
class SelfRule : public IGrammarRule
{
  public:
    std::unique_ptr<Expr> parse(ParserContext &ctx) override;

    std::string toStr(GrammarRuleHashTable &visited) const override { return "Self"; }
};
