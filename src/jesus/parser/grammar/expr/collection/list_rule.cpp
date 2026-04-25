#include "list_rule.hpp"
#include "ast/expr/list_expr.hpp"
#include "ast/stmt/incomplete_block_stmt.hpp"

std::unique_ptr<Expr> ListRule::parse(ParserContext &ctx)
{
    if (!ctx.match(TokenType::LEFT_BRACKET))
        return nullptr;

    std::vector<std::unique_ptr<Expr>> elements;

    ctx.consumeAllNewLines();

    if (!ctx.check(TokenType::RIGHT_BRACKET))
    {
        do
        {
            ctx.consumeAllNewLines();

            auto value = expression->parse(ctx);
            if (!value)
            {
                throw std::runtime_error("Expected value inside list.");
            }

            elements.push_back(std::move(value));

            ctx.consumeAllNewLines();

        } while (ctx.match(TokenType::COMMA));
    }

    if (!ctx.match(TokenType::RIGHT_BRACKET))
    {
        throw std::runtime_error("Expected ']' to close list.");
    }

    return std::make_unique<ListExpr>(std::move(elements));
}
