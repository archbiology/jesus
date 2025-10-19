#include "equality_rule.hpp"
#include "../../../../../ast/expr/literal_expr.hpp"
#include "../../../../../ast/expr/binary_expr.hpp"
#include "../../../../../ast/expr/parity_check_expr.hpp"
#include "../../../../../types/known_types.hpp"

std::unique_ptr<Expr> EqualityRule::parse(ParserContext &ctx)
{
    auto left = operand->parse(ctx);
    if (!left)
        return nullptr;

    int snapshot = ctx.snapshot();
    if (ctx.match(TokenType::EQUAL) && ctx.match(TokenType::EQUAL))
    {
        throw std::runtime_error("Use 'is' for equality instead of '=='.");
    }

    ctx.restore(snapshot);
    while (ctx.match(TokenType::IS))
    {
        Token op = ctx.previous();

        // ----------------------------------
        // Handle optional 'not' for 'is not'
        // ----------------------------------
        bool isNegated = false;
        if (ctx.match(TokenType::NOT))
        {
            isNegated = true;
            // Combine tokens into a single virtual operator 'is not'
            op.lexeme = "is not";
            op.type = TokenType::NOT_EQUAL; // Treat as inequality internally
        }

        // ---------------------------------------
        // Handle “is [not] odd/even” immediately
        // ---------------------------------------
        if (ctx.match(TokenType::ODD))
        {
            return std::make_unique<ParityCheckExpr>(std::move(left), isNegated, true);
        }
        else if (ctx.match(TokenType::EVEN))
        {
            return std::make_unique<ParityCheckExpr>(std::move(left), isNegated, false);
        }

        if (ctx.match(TokenType::A))
        {
            if (!ctx.match(TokenType::IDENTIFIER))
                throw std::runtime_error("Expected a type name after 'a/an'.");

            std::string varType_ = ctx.previous().lexeme;

            auto varType = KnownTypes::resolve(varType_, "core"); // std::shared_ptr<CreationType>
            if (!varType)
            {
                throw std::runtime_error("Unknown type: '" + varType_ + "'.");
            }

            auto exprType = left->getReturnType(ctx);
            bool result = exprType->isA(varType);
            if (isNegated)
                result = !result;

            // FIXME: When inheritance is supported, use TypeCheckExpr instead of LiteralExpr
            // return std::make_unique<TypeCheckExpr>(std::move(left), varType, isNegated);
            return std::make_unique<LiteralExpr>(Value(result), KnownTypes::TRUTH);
        }

        // -------------------------------
        // Otherwise it's a plain equality
        // -------------------------------
        if (!isNegated)
        {
            op.type = TokenType::IS; // Equivalent to equality (== in other languages)
        }

        auto right = operand->parse(ctx);
        if (!right)
        {
            std::cerr << "Expected right-hand expression after '" << op.lexeme << "' equality operator.\n";
            return nullptr;
        }

        left = std::make_unique<BinaryExpr>(std::move(left), op, std::move(right));
    }

    return left;
}

std::string EqualityRule::toStr(GrammarRuleHashTable &visited) const
{
    if (visited.count(this))
        return "Equality(...)";

    visited.insert(this);

    return "Equality(" + operand->toStr(visited) + ")";
}
