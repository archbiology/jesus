#include "convert_to_rule.hpp"
#include "../../../../../ast/expr/convert_to_expr.hpp"
#include "../../../../../types/known_types.hpp"

std::unique_ptr<Expr> ConvertToRule::parse(ParserContext &ctx)
{

    auto left = previousRule->parse(ctx);

    if (ctx.match(TokenType::TO))
    {
        if (!ctx.match(TokenType::IDENTIFIER))
            throw std::runtime_error("Expected type name after 'to'.");

        auto typeName = ctx.previous().lexeme;
        auto targetType = KnownTypes::resolve(typeName, "core");
        if (!targetType)
            throw std::runtime_error("Unknown type to convert to: " + typeName);

        // -------------
        // Type checking
        // -------------
        auto sourceType = left->getReturnType(ctx);
        if (sourceType == targetType)
        {
            // No conversion needed — same type
            return left;
        }

        if (sourceType->isA(targetType))
        {
            // child -> parent (upcast)
            return left;
        }

        if (targetType->isA(sourceType))
        {
            // parent -> child (downcast)
            return std::make_unique<ConvertToExpr>(std::move(left), targetType);
        }

        // Otherwise — unrelated types
        throw std::runtime_error("Cannot convert from '" + sourceType->name + "' to '" + targetType->name + "' since they share no inheritance.");
    }

    return left;
}
