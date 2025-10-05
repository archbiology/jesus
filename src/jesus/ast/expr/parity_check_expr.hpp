#pragma once

#include "expr.hpp"
#include <memory>

/**
 * @brief Represents expressions like `x is odd` or `y is not even`.
 *
 * Internally, this is equivalent to a modulo check: (x mod 2 == 1) or (x mod 2 == 0).
 * It simplifies common numeric parity tests into natural language syntax.
 *
 * "He has made everything beautiful in its time." — Ecclesiastes 3:11
 */
class ParityCheckExpr : public Expr
{
public:
    std::unique_ptr<Expr> target;
    bool negate;
    bool checkOdd;

    ParityCheckExpr(std::unique_ptr<Expr> target, bool negate, bool checkOdd)
        : target(std::move(target)), negate(negate), checkOdd(checkOdd) {}

    Value evaluate(std::shared_ptr<Heart> heart) const override
    {
        Value val = target->evaluate(heart);

        if (!val.IS_NUMBER)
            // FIXME: Check this at PARSE time, not RUNTIME
            throw std::runtime_error("❌ Error: Parity checks ('odd'/'even') only support integer types.");

        int n = val.toInt();
        bool result = checkOdd ? (n % 2 != 0) : (n % 2 == 0);

        if (negate)
            result = !result;

        return Value(result);
    }

    Value accept(ExprVisitor &visitor) const override;

    bool canEvaluateAtParseTime() const override
    {
        return target->canEvaluateAtParseTime();
    }

    std::shared_ptr<CreationType> getReturnType(ParserContext &ctx) const override;

    std::string toString() const override
    {
        std::string desc = "ParityCheckExpr(";
        desc += "" + target->toString();
        desc += (negate ? "is not" : "is");
        desc += (checkOdd ? "odd" : "even");

        desc += ")";
        return desc;
    }
};
