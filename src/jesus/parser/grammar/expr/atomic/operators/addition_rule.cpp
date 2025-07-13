#include "addition_rule.hpp"
#include "../../../../../ast/expr/binary_expr.hpp"

std::unique_ptr<Expr> AdditionRule::parse(ParserContext &ctx)
{
    auto left = inner->parse(ctx);
    if (!left)
        return nullptr;

    while (ctx.matchAny({TokenType::PLUS, TokenType::MINUS}))
    {
        Token op = ctx.previous();
        auto right = inner->parse(ctx);
        if (!right)
        {
            std::cerr << "Expected expression after '+' or '-' in AdditionRule.\n";
            return nullptr;
        }

        left = std::make_unique<BinaryExpr>(std::move(left), op, std::move(right));
    }

    return left;
}
