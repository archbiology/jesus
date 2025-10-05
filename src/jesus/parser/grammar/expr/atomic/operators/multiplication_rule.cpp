#include "multiplication_rule.hpp"
#include "../../../../../ast/expr/binary_expr.hpp"

std::unique_ptr<Expr> MultiplicationRule::parse(ParserContext &ctx)
{
    auto left = unary->parse(ctx);
    if (!left)
        return nullptr;

    while (ctx.matchAny({TokenType::STAR, TokenType::SLASH, TokenType::MOD}))
    {
        Token op = ctx.previous();
        auto right = unary->parse(ctx);
        if (!right)
            return nullptr;

        left = std::make_unique<BinaryExpr>(std::move(left), op, std::move(right));
    }

    return left;
}

std::string MultiplicationRule::toStr(GrammarRuleHashTable &visited) const
{
    if (visited.count(this))
        return "Multiplication(...)";

    visited.insert(this);

    return "Multiplication(" + unary->toStr(visited) + ")";
}
