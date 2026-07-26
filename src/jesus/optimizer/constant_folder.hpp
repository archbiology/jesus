#pragma once

#include <vector>
#include <memory>
#include "ast/stmt/stmt.hpp"
#include "ast/expr/expr.hpp"
#include "ast/expr/binary_expr.hpp"

/**
 * @brief Replaces compile-time constant expressions with their computed values.
 *
 * Example:
 *
 *     create number result = (2 + 3) * 4
 *
 * becomes:
 *
 *     create number result = 20
 *
 * "When you beat the olives from your trees, do not go over the branches a second time.
 * Leave what remains for the foreigner, the fatherless and the widow."
 * — Deuteronomy 24:20
 */
class ConstantFolder
{
  public:
    void run(std::vector<std::unique_ptr<Stmt>> &program);

  private:
    void optimizeStatement(Stmt &statement);

    std::unique_ptr<Expr> optimizeExpression(std::unique_ptr<Expr> expression);

    std::unique_ptr<Expr> optimizeBinaryExpr(std::unique_ptr<BinaryExpr> expression);

    std::unique_ptr<Expr> createLiteral(const Value &value);
};
