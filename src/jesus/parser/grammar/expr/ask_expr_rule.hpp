#pragma once
#include "../grammar_rule.hpp"

/**
 * @brief Grammar rule for parsing `ask` expressions in the Jesus language.
 *
 * It parses expressions that use the `ask` keyword, which prompts the user
 * for input at runtime and stores the result in a variable.
 *
 * In typical usage, the `ask` keyword is followed by a string literal or a
 * text-typed variable that serves as the prompt message. This rule ensures
 * the correct syntax is enforced.
 *
 * Example Jesus code:
 * @code
 *  create int age = ask "What is your age?"
 * @endcode
 *
 * "Ask, and it will be given to you; seek, and you will find; knock, and it will be opened to you." — Matthew 7:7
 */
class AskExprRule : public IGrammarRule
{
public:
    std::unique_ptr<Expr> parse(ParserContext &ctx) override;

    std::string toStr(GrammarRuleHashTable &visited) const override;
};
