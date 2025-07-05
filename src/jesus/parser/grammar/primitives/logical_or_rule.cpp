#include "logical_or_rule.hpp"
#include "../../../ast/expr/binary_expr.hpp"

std::unique_ptr<Expr> LogicalOrRule::parse(ParserContext &ctx)
{
    auto left = nextRule->parse(ctx);
    if (!left)
        return nullptr;

    while (ctx.match(TokenType::OR))
    {
        Token op = ctx.previous();
        auto right = nextRule->parse(ctx);
        if (!right)
        {
            std::cerr << "Expected right-hand expression after '" << op.lexeme << "' OR operator.\n";
            return nullptr;
        }

        left = std::make_unique<BinaryExpr>(std::move(left), op, std::move(right));
    }

    return left;
}
