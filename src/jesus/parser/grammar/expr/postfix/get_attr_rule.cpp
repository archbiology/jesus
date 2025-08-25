#include "get_attr_rule.hpp"
#include "../../../../ast/expr/get_attr_expr.hpp"
#include <memory>

std::unique_ptr<Expr> GetAttributeRule::parse(ParserContext &ctx)
{
    // Start with a primary expression
    auto expr = primary->parse(ctx);
    if (!expr)
        return nullptr;

    // Parse more identifiers for attribute access: say person name
    while (ctx.match(TokenType::IDENTIFIER))
    {
        std::string attribute = ctx.previous().lexeme;
        expr = std::make_unique<GetAttributeExpr>(std::move(expr), attribute);
    }
    return expr;
}

std::string GetAttributeRule::toStr(GrammarRuleHashTable &visited) const
{
    if (visited.count(this))
        return "GetAttributeExprRule(...)";

    visited.insert(this);

    return "GetAttributeExprRule()";
}
