#include "group_rule.hpp"
#include "../../ast/expr/grouping_expr.hpp"

std::unique_ptr<Expr> GroupRule::parse(ParserContext &ctx)
{
    if (!ctx.match(TokenType::LEFT_PAREN))
        return nullptr;

    auto expr = inner->parse(ctx);
    if (!expr)
        return nullptr;

    if (!ctx.match(TokenType::RIGHT_PAREN))
        throw std::runtime_error("Expected ')' after expression.");

    return std::make_unique<GroupingExpr>(std::move(expr));
}
