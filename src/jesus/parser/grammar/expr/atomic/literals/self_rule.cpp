#include "self_rule.hpp"
#include "variable_rule.hpp"
#include "ast/expr/variable_expr.hpp"
#include "parser/parser_context.hpp"
#include "spirit/heart.hpp"

std::unique_ptr<Expr> SelfRule::parse(ParserContext &ctx)
{
    if (ctx.match(TokenType::MY))
    {
        if (!ctx.varExistsInHierarchy(SELF_VARIABLE))
        {
            throw std::runtime_error(
                "'I'/'my' can only be used inside a method body to refer to the "
                "object that method is running on. There is no current object here.");
        }

        auto address = ctx.resolveVariableAddress(SELF_VARIABLE);
        return std::make_unique<VariableExpr>(address, SELF_VARIABLE);
    }

    return nullptr;
}
