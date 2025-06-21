
#include "expr.hpp"
#include <memory>

/**
 * @brief Represents conditional expressions like the following:
 *   say "adult" if age >= 18 otherwise "young"
 *
 * 📖 "I have set before you life and death, blessings and curses. Now choose life, so that you and your children may live."
 * — Deuteronomy 30:19
 *
 * This node evaluates a condition and returns the result of either the
 * `thenBranch` or `elseBranch` branch based on the logic outcome of the condition.
 *
 * It enables decision-making logic in the language's syntax, allowing
 * expressions such as: condition ? thenBranch : elseBranch
 */
class ConditionalExpr : public Expr
{
public:
    std::unique_ptr<Expr> condition;
    std::unique_ptr<Expr> thenBranch;

    std::unique_ptr<Expr> elseBranch;

    /**
     * @brief Construct a new ConditionalExpr object
     *
     * Represents a conditional expression (similar to a ternary operation).
     *
     * 📖 "I have set before you life and death, blessings and curses. Now choose life, so that you and your children may live."
     * — Deuteronomy 30:19
     *
     * @param condition The condition to evaluate (should return "true" or "false").
     * @param thenBranch The expression to evaluate if the condition is true.
     * @param elseBranch The expression to evaluate if the condition is false.
     */
    ConditionalExpr(std::unique_ptr<Expr> condition,
                    std::unique_ptr<Expr> thenBranch,
                    std::unique_ptr<Expr> elseBranch)
        : condition(std::move(condition)),
          thenBranch(std::move(thenBranch)),
          elseBranch(std::move(elseBranch)) {}

  /**
   * @brief  Evaluates a condition and returns the result of either the
   * `thenBranch` or `elseBranch` branch based on the logic outcome of the condition.
   *
   * - If the condition evaluates to "true", it evaluates and returns the `thenBranch` branch.
   * - If the condition evaluates to anything else (or fails), it evaluates and returns the `elseBranch` branch if available.
   *
   * @param heart The "Symbol table"
   * @return std::optional<std::string>
   */
  Value evaluate(Heart *heart) override
  {
    Value result = condition->evaluate(heart);

    if (result.AS_BOOLEAN)
    {
      return thenBranch->evaluate(heart);
    }

    if (elseBranch)
    {
      return elseBranch->evaluate(heart);
    }

    return Value::formless();
  }

  /**
   * @brief Returns a string representation of the expression.
   *
   * "For nothing is hidden that will not be made manifest, nor is anything
   * secret that will not be known and come to light." — Luke 8:17
   */
  virtual std::string toString() const override
    {
        std::string str = "ConditionalExpr";

        if (condition)
            str += "(condition: " + condition->toString();
        if (thenBranch)
            str += ", thenBranch: " + thenBranch->toString();
        if (elseBranch)
            str += ", elseBranch: " + elseBranch->toString();

        str += ")";

        return str;
    }
};
