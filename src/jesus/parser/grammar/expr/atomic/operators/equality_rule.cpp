#include "equality_rule.hpp"
#include "../../../../../ast/expr/binary_expr.hpp"
#include "../../../../../ast/expr/parity_check_expr.hpp"

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
