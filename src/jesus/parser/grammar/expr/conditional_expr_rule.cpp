#include "conditional_expr_rule.hpp"
#include "../../../ast/expr/conditional_expr.hpp"
#include <iostream>

std::unique_ptr<Expr> ConditionalExprRule::parse(ParserContext &ctx)
{
    int backup = ctx.snapshot();

    // Parse the "then" expression
    auto ifExpr = expression->parse(ctx);
    if (!ifExpr)
    {
        ctx.restore(backup);
        return nullptr;
    }

    // Match "if"
    if (!ctx.match(TokenType::IF))
    {
        return ifExpr;
    }

    // Parse the condition expression
    auto condition = expression->parse(ctx);
    if (!condition)
    {
        std::cerr << "Expected condition after 'if'.\n";
        ctx.restore(backup);
        return nullptr;
    }

    // Match "otherwise"
    if (!ctx.match(TokenType::OTHERWISE))
    {
        std::cerr << "Expected 'otherwise' after condition.\n";
        ctx.restore(backup);
        return nullptr;
    }

    // Parse the else/otherwise expression
    auto otherwiseExpr = expression->parse(ctx);
    if (!otherwiseExpr)
    {
        std::cerr << "[ConditionalExprRule] Expected expression after 'otherwise'.\n";
        ctx.restore(backup);
        return nullptr;
    }

    return std::make_unique<ConditionalExpr>(
        std::move(condition),
        std::move(ifExpr),
        std::move(otherwiseExpr));
}

std::string ConditionalExprRule::toStr(GrammarRuleHashTable &visited) const
{
    if (visited.count(this))
        return "ConditionalExpr(...)";

    visited.insert(this);

    return "ConditionalExpr(" + expression->toStr(visited) + ")";
}
