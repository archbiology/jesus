#pragma once

#include <vector>
#include <memory>
#include <unordered_set>
#include <unordered_map>
#include <string>

#include "ast/stmt/stmt.hpp"
#include "ast/expr/expr.hpp"
#include "ast/expr/literal_expr.hpp"

/**
 * @brief Replaces reads of immutable variables with their constant values.
 *
 * Constant propagation is an optimization performed after ConstantFolder.
 * It discovers variables whose values never change and substitutes later
 * references to those variables with the corresponding constant expression.
 *
 * Example:
 *
 *     create number age = 33
 *     say age
 *
 * becomes:
 *
 *     create number age = 33
 *     say 33
 *
 * The declaration itself is intentionally preserved. This pass only replaces
 * later reads of the variable. Removing the declaration is the responsibility
 * of DeadCodeEliminator.
 *
 * To guarantee correctness, only variables that are never modified are
 * propagated. Variables updated through assignments, loops, user input,
 * object mutation, or any other write operation are excluded.
 *
 * Pipeline:
 *
 *     ConstantFolder
 *         ↓
 *     ConstantPropagator
 *         ↓
 *     DeadCodeEliminator
 *
 * "The simple inherit folly, but the prudent are crowned with knowledge."
 * — Proverbs 14:18
 */
class ConstantPropagator
{
  public:
    /**
     * @brief Executes constant propagation on an AST.
     *
     * The optimization is intentionally divided into two passes.
     *
     * Pass 1:
     *   Walk the entire program and discover which variables may change.
     *   Any variable that is reassigned, written by user input, modified inside
     *   loops, or otherwise mutated is marked as non-constant.
     *
     * Pass 2:
     *   Walk the program again. Whenever a read of a known constant variable is
     *   found, replace that VariableExpr with an equivalent LiteralExpr.
     */
    void run(std::vector<std::unique_ptr<Stmt>> &program);

  private:
    void collectModifiedVars(
            const Stmt &statement,
            std::unordered_set<std::string> &modifiedVars,
            std::unordered_set<std::string> &declaredVars);

    void collectModifiedVarsFromExpr(const Expr *expression, std::unordered_set<std::string> &modifiedVars);

    void replaceConstWithLiteralInStatement(
            Stmt &statement, const std::unordered_map<std::string, std::unique_ptr<Expr>> &constVars);

    /**
     * @brief This is the heart of ConstantPropagator.
     *
     * If this expression is a variable, and I know its constant value,
     *    replace the variable by a copy of that constant.
     * Otherwise,
     *    leave the expression unchanged.
     */
    std::unique_ptr<Expr> replaceConstWithLiteralInExpression(
            std::unique_ptr<Expr> expression, const std::unordered_map<std::string, std::unique_ptr<Expr>> &constVars);

    std::unique_ptr<Expr> cloneExpr(const Expr &expr);
    std::unique_ptr<Expr> createLiteral(const Value &value);
};
