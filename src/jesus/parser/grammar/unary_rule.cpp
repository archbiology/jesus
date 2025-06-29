#include "unary_rule.hpp"
#include "../../lexer/token_type.hpp"

bool UnaryRule::parse(ParserContext &ctx)
{
    if (ctx.matchAny({TokenType::NOT, TokenType::MINUS, TokenType::PLUS}))
    {
        // Recursively call self to support cases like -(--42)
        if (!parse(ctx))
            return false;

        return true;
    }

    // Try to parse operand directly without unary operator.
    return operandRule->parse(ctx);
}

std::string UnaryRule::toStr(GrammarRuleHashTable &visitedTable) const
{
    if (visitedTable.count(this))
        return "Unary(...)";

    visitedTable.insert(this);

    return "Unary(" + operandRule->toStr(visitedTable) + ")";
}
