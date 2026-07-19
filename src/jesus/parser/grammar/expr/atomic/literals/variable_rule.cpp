#include "variable_rule.hpp"
#include "../../../../../ast/expr/variable_expr.hpp"

std::unique_ptr<Expr> VariableRule::parse(ParserContext &ctx)
{
    if (ctx.match(TokenType::IDENTIFIER))
    {
        auto varname = ctx.previous().lexeme;
        auto address = ctx.resolveVariableAddress(varname);
        return std::make_unique<VariableExpr>(address, varname);
    }

    return nullptr;
}
