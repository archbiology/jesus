#include "equality_rule.hpp"
#include "../../../ast/expr/binary_expr.hpp"

std::unique_ptr<Expr> EqualityRule::parse(ParserContext &ctx)
{
    auto left = operand->parse(ctx);
    if (!left)
        return nullptr;

    while (ctx.matchAny({TokenType::EQUAL_EQUAL, TokenType::NOT_EQUAL}))
    {
        Token op = ctx.previous();
        auto right = operand->parse(ctx);
        if (!right)
        {
            std::cerr << "Expected right-hand expression after '" << op.lexeme << "' equality operator.\n";
            return nullptr;
        }

        left = std::make_unique<BinaryExpr>(std::move(left), op, std::move(right));
    }

    return left;
}

std::string EqualityRule::toStr(GrammarRuleHashTable &visited) const
{
    if (visited.count(this))
        return "Equality(...)";

    visited.insert(this);

    return "Equality(" + operand->toStr(visited) + ")";
}
