
#include "expr.hpp"
#include <memory>

class ConditionalExpr : public Expr
{
public:
    std::unique_ptr<Expr> condition;
    std::unique_ptr<Expr> thenBranch;

    std::unique_ptr<Expr> elseBranch;

    ConditionalExpr(std::unique_ptr<Expr> condition,
                    std::unique_ptr<Expr> thenBranch,
                    std::unique_ptr<Expr> elseBranch)
        : condition(std::move(condition)),
          thenBranch(std::move(thenBranch)),
          elseBranch(std::move(elseBranch)) {}

  /**
   * @brief  Evaluates a condition and returns the result of either the
   * `ifTrue` or `ifFalse` branch based on the logic outcome of the condition.
   *
   * - If the condition evaluates to "true", it evaluates and returns the `ifTrue` branch.
   * - If the condition evaluates to anything else (or fails), it evaluates and returns the `ifFalse` branch if available.
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
            str += ", ifTrue: " + thenBranch->toString();
        if (elseBranch)
            str += ", ifFalse: " + elseBranch->toString();

        str += ")";

        return str;
    }
};
