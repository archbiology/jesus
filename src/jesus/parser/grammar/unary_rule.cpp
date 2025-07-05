#include "unary_rule.hpp"
#include "../../ast/expr/unary_expr.hpp"

std::unique_ptr<Expr> UnaryRule::parse(ParserContext &ctx)
{
    if (ctx.matchAny({TokenType::NOT, TokenType::MINUS, TokenType::PLUS}))
    {
        Token op = ctx.previous();

        auto right = parse(ctx); // Recursive call to allow repeated like `-(--42)`
        if (!right)
            return nullptr;

        return std::make_unique<UnaryExpr>(op, std::move(right));
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
