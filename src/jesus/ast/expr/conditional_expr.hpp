
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
     * @brief Returns a string representation of the expression.
     *
     * "For nothing is hidden that will not be made manifest, nor is anything
     * secret that will not be known and come to light." — Luke 8:17
     */
    virtual std::string toString() const override { return "ConditionalExpr"; }
};
