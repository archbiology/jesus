#include "list_rule.hpp"
#include "ast/expr/list_expr.hpp"
#include "ast/stmt/incomplete_block_stmt.hpp"
#include "types/known_types.hpp"

std::unique_ptr<Expr> ListRule::parse(ParserContext &ctx)
{
    if (!ctx.match(TokenType::LEFT_BRACKET))
        return nullptr;

    std::vector<std::unique_ptr<Expr>> elements;
    std::shared_ptr<CreationType> elementType = nullptr;

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

            auto currentType = value->getReturnType(ctx);
            elements.push_back(std::move(value));

            ctx.consumeAllNewLines();

            /*
             * ------------------------------------------------------------
             * List type inference
             * ------------------------------------------------------------
             *
             * Lists infer their element type automatically during parsing.
             *
             * Rules:
             *
             *   []                  -> list<creation>
             *   [1, 2, 3]           -> list<int>
             *   ['a', 'b']          -> list<text>
             *   [user1, user2]      -> list<User>
             *   [1, 'abc']          -> list<creation>
             *
             * Inference strategy:
             *
             * - The first element defines the initial list type.
             * - If all elements share the same type, the list keeps that type.
             * - If a different type appears, the list falls back to `creation`.
             *
             * This allows Python/JavaScript-like list syntax while preserving
             * strong typing whenever possible.
             * ------------------------------------------------------------
             */
            if (!elementType)
            {
                elementType = currentType;
            }
            else if (
                elementType != KnownTypes::CREATION &&
                currentType != elementType)
            {
                elementType = KnownTypes::CREATION;
            }

        } while (ctx.match(TokenType::COMMA));
    }

    if (!ctx.match(TokenType::RIGHT_BRACKET))
    {
        throw std::runtime_error("Expected ']' to close list.");
    }

    // Empty list defaults to creation
    if (!elementType)
        elementType = KnownTypes::CREATION;

    auto listType = KnownTypes::makeListType(elementType);

    return std::make_unique<ListExpr>(std::move(elements), listType);
}
