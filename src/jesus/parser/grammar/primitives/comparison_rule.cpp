#include "comparison_rule.hpp"
#include "../../../ast/expr/binary_expr.hpp"

std::unique_ptr<Expr> ComparisonRule::parse(ParserContext &ctx)
{
    auto left = inner->parse(ctx);
    if (!left)
        return nullptr;

    while (ctx.matchAny({TokenType::GREATER,
                         TokenType::GREATER_EQUAL,
                         TokenType::LESS,
                         TokenType::LESS_EQUAL}))
    {
        Token op = ctx.previous();
        auto right = inner->parse(ctx);
        if (!right)
        {
            std::cerr << "Expected right-hand expression after '" << op.lexeme << "' comparison operator.\n";
            return nullptr;
        }

        left = std::make_unique<BinaryExpr>(std::move(left), op, std::move(right));
    }

    return left;
}

std::string ComparisonRule::toStr(GrammarRuleHashTable &visited) const
{
    if (visited.count(this))
        return "Comparison(...)";

    visited.insert(this);

    return "Comparison(" + inner->toStr(visited) + ")";
}
