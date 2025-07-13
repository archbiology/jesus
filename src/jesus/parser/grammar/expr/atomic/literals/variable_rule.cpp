#include "variable_rule.hpp"
#include "../../../../../ast/expr/variable_expr.hpp"

std::unique_ptr<Expr> VariableRule::parse(ParserContext &ctx)
{
    if (ctx.match(TokenType::IDENTIFIER))
        return std::make_unique<VariableExpr>(ctx.previous().lexeme);

    return nullptr;
}
