#pragma once
#include "../../grammar_rule.hpp"

/**
 * @brief Grammar rule for accessing attributes of an object.
 *
 * This rule allows parsing expressions where a primary expression
 * (like a variable or an instance) is followed by one or more
 * identifiers, representing attribute access.
 *
 * Example usage in the interpreted language:
 * @code
 * say person name
 * @endcode
 *
 * The rule supports chained attribute access:
 * @code
 * say person address city
 * @endcode
 *
 * "For where your treasure is, there your heart will be also." — Matthew 6:21
 */
class GetAttributeRule : public IGrammarRule
{
    /**
     * @brief The base grammar rule that produces the object to access attributes from
     */
    std::shared_ptr<IGrammarRule> primary;

public:
    /**
     * @brief Constructs a GetAttributeRule with a given primary expression rule.
     *
     * @param primary The rule that produces the object whose attributes will be accessed.
     */
    explicit GetAttributeRule(std::shared_ptr<IGrammarRule> primary)
        : primary(std::move(primary)) {}

    /**
     * @brief Parses the attribute access expression.
     *
     * This method starts by parsing the primary expression and then
     * continues parsing any subsequent identifiers as attribute accesses,
     * returning a sequential chain of GetAttributeExpr nodes.
     *
     * @param ctx The parser context containing the tokens and current parsing position.
     * @return A unique pointer to the resulting expression AST node.
     */
    std::unique_ptr<Expr> parse(ParserContext &ctx) override;

    std::string toStr(GrammarRuleHashTable &visited) const override;
};
