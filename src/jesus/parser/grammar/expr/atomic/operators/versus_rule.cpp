#include "versus_rule.hpp"
#include "../../../../../ast/expr/binary_expr.hpp"

VersusRule::VersusRule(std::shared_ptr<IGrammarRule> lhs, std::shared_ptr<IGrammarRule> rhs)
    : lhsRule(std::move(lhs)), rhsRule(std::move(rhs))
{
}

std::unique_ptr<Expr> VersusRule::parse(ParserContext &ctx)
{
    auto left = lhsRule->parse(ctx);
    if (!left)
        return nullptr;

    while (ctx.match(TokenType::VERSUS))
    {
        Token op = ctx.previous();
        auto right = rhsRule->parse(ctx);
        if (!right)
        {
            std::cerr << "Expected right-hand expression after '" << op.lexeme << "' VERSUS operator.\n";
            return nullptr;
        }

        left = std::make_unique<BinaryExpr>(std::move(left), op, std::move(right));
    }

    return left;
}

std::string VersusRule::toStr(GrammarRuleHashTable &visitedTable) const
{
    if (visitedTable.count(this))
        return "Versus(...)";

    visitedTable.insert(this);

    return "Versus(" + lhsRule->toStr(visitedTable) + " vs " + rhsRule->toStr(visitedTable) + ")";
}
