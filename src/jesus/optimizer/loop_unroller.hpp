#pragma once

#include <vector>
#include <memory>
#include "ast/stmt/stmt.hpp"
#include "ast/expr/expr.hpp"
#include "ast/expr/binary_expr.hpp"

class AssignableExpr;  // Forward declaration
class RepeatTimesStmt; // Forward declaration

/**
 * @brief Unrolls small constant repeat loops into repeated statements.
 *
 * A `repeat N times` loop is replaced by N copies of its body when
 * the loop is small enough to make unrolling worthwhile.
 *
 * Example:
 *
 *     repeat 3 times:
 *         say 'Yes'
 *     amen
 *
 * becomes:
 *
 *     say 'Yes'
 *     say 'Yes'
 *     say 'Yes'
 *
 * "And the scroll of the prophet Isaiah was given to him.
 * He unrolled the scroll and found the place where it was written:"
 * — Luke 4:17
 */
class LoopUnroller
{
  public:
    /**
     *  LoopUnroller::run
     *  │
     *  ├── find loops
     *  ├── decide whether a loop should be unrolled
     *  ├── clone statements
     *  └── clone expressions
     */
    void run(std::vector<std::unique_ptr<Stmt>> &program);

  private:
    static constexpr size_t MAX_UNROLL_COUNT = 7;
    static constexpr size_t MAX_UNROLL_BODY_STATEMENTS = 7;

    void unrollBlock(std::vector<std::unique_ptr<Stmt>> &stmts);
    std::vector<std::unique_ptr<Stmt>> unrollStatement(std::unique_ptr<Stmt> statement);

    // -----------------------
    // Loop unrolling helpers
    // -----------------------
    /**
     * @brief Convert to int ONLY if it is a literal int expression
     */
    int literalCount(const Expr *countExpr) const;

    /**
     * @brief Decide whether a repeat-times loop is small enough to unroll.
     *
     * We deliberately use simple limits instead of trying to estimate
     * AST complexity or runtime cost.
     *
     * Unrolling is allowed only when:
     *
     *   1. The iteration count is a positive integer literal.
     *   2. The loop runs at most 7 times.
     *   3. The loop body contains at most 7 statements.
     *   4. Every statement can be safely cloned.
     *
     * This keeps loop unrolling predictable and prevents large AST growth.
     *
     * Example:
     *
     *     repeat 3 times:
     *         say "yes"
     *         say "again"
     *     amen
     *
     * is small enough to become:
     *
     *     say "yes"
     *     say "again"
     *     say "yes"
     *     say "again"
     *     say "yes"
     *     say "again"
     *
     * But:
     *
     *     repeat 8 times:
     *         ...
     *
     * is never unrolled, regardless of how small its body is.
     *
     * Statements that can be extracted
     *    say/warn
     *    CreateVar
     *    UpdateVar
     *    AssignStmt (update attributes and indexes)
     *    CreateVarWithAsk
     *    UpdateVarWithAsk
     *    InspectStmt
     *    ImportModuleStmt (FIXME: this should ideally not be allowed inside loops)
     *    ServeStmt
     *    CreateVarTypeStmt (FIXME: this should ideally not be allowed inside loops)
     */
    bool shouldUnroll(const RepeatTimesStmt &loop) const;
    std::unique_ptr<Stmt> cloneStatement(const Stmt &statement) const;

    /**
     * @brief Clone IndexExpr and GetAttributeExpr, who inherits from AssignableExpr
     */
    std::unique_ptr<AssignableExpr> cloneAssignableExpr(const AssignableExpr &target) const;

    /**
     * @brief
     *
     * FIXME: When adding new expressions, consider them in this method.
     * Instead of having to remember, add a clone() method to Expr where
     * every child to implement. This way, we don't have to remember it.
     */
    std::unique_ptr<Expr> cloneExpression(const Expr &expression) const;
};
