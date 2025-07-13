#include "logical_and_rule.hpp"
#include "../../../../../ast/expr/binary_expr.hpp"

std::unique_ptr<Expr> LogicalAndRule::parse(ParserContext &ctx)
{
    auto left = nextRule->parse(ctx);
    if (!left)
        return nullptr;

    while (ctx.match(TokenType::AND))
    {
        Token op = ctx.previous();
        auto right = nextRule->parse(ctx);
        if (!right)
        {
            std::cerr << "Expected right-hand expression after '" << op.lexeme << "' logical AND operator.\n";
            return nullptr;
        }

        left = std::make_unique<BinaryExpr>(std::move(left), op, std::move(right));
    }

    return left;
}

std::string LogicalAndRule::toStr(GrammarRuleHashTable &visited) const
{
    if (visited.count(this))
        return "LogicalAnd(...)";

    visited.insert(this);

    return "LogicalAnd(" + nextRule->toStr(visited) + ")";
}
