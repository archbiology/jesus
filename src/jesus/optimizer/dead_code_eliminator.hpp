#pragma once

#include <vector>
#include <memory>
#include <unordered_set>
#include <string>

#include "ast/stmt/stmt.hpp"
#include "ast/expr/expr.hpp"

class CreateVarStmt; // Forward declaration
class UpdateVarStmt;
class AssignStmt;
class CreateClassStmt;
class MethodCallExpr;

/**
 * @brief Eliminates dead code from the AST.
 *
 * Dead code elimination is an optimization performed after ConstantFolder and ConstantPropagator.
 * It removes statements that will never execute (such as dead branches in if-statements,
 * dead loops, or statements after return/break/skip) as well as variable declarations/assignments
 * whose values are never read.
 *
 * This pass is intended to run after constant folding and constant
 * propagation, since those optimizations often expose additional dead code.
 *
 * Example:
 *
 *     create number age = 33
 *     say 33
 *
 * becomes:
 *
 *     say 33
 *
 * Pipeline:
 *
 *     ConstantFolder
 *         ↓
 *     ConstantPropagator
 *         ↓
 *     DeadCodeEliminator
 *
 * "Every branch in me that does not bear fruit he takes away,
 * and every branch that does bear fruit he prunes, that it may bear more fruit."
 * — John 15:2
 */
class DeadCodeEliminator
{
  public:
    /**
     * @brief Removes dead code from the program.
     *
     * The AST is modified in place.
     *
     * Internal flow:
     *
     *    run()
     *       ↓
     *  collectReferencedVariables()
     *  collectReferencedClasses()
     *       ↓
     *  optimizeBlock()
     *       ↓
     *  rewriteStatement()
     *       ↓
     *  canDiscard()
     *       ↓
     *  hasNoSideEffects()
     */
    void run(std::vector<std::unique_ptr<Stmt>> &program);

  private:
    void collectReferencedVariables(const Stmt &statement, std::unordered_set<std::string> &usedVars);
    void collectReferencedVariables(const Expr *expression, std::unordered_set<std::string> &usedVars);

    void collectReferencedClasses(const Stmt &statement, std::unordered_set<std::string> &usedClasses);
    void collectReferencedClasses(const Expr *expression, std::unordered_set<std::string> &usedClasses);

    /**
     * @brief Check if expression has no side effect (LiteralExpr or VariableExpr)
     *
     * Reading a literal or a variable has no observable side effects.
     * Evaluating them cannot modify program state, perform I/O, or throw,
     * so they are safe to eliminate when their value is unused.
     *
     * IMPORTANT:
     * Whenever a new Expr subclass is introduced, this function should be
     * reviewed to determine whether that expression is pure or has side effects.
     */
    bool hasNoSideEffects(const Expr *expression);

    /**
     * @brief Check if a statement has no side effects.
     */
    bool hasNoSideEffects(const Stmt *stmt);

    /**
     * @brief Returns whether the statement can be safely removed.
     *
     * A declaration or assignment can be discarded when the assigned value
     * is never read and evaluating that value has no side effects.
     */
    bool canDiscard(const CreateVarStmt *, const std::unordered_set<std::string> &usedVars);
    bool canDiscard(const UpdateVarStmt *, const std::unordered_set<std::string> &usedVars);
    bool canDiscard(const AssignStmt *, const std::unordered_set<std::string> &usedVars);
    bool canDiscard(const CreateClassStmt *, const std::unordered_set<std::string> &usedClasses);

    /**
     * @brief Check if statement is return, break, skip, or resist.
     */
    bool isControlFlowTerminator(const Stmt &statement);

    /**
     * @brief Returns whether a loop body ends with an unconditional skip.
     *
     * If that is the case, it can be safely just removed
     */
    bool endsWithUnconditionalSkip(const std::vector<std::unique_ptr<Stmt>> &body);

    /**
     * @brief Returns whether a loop body ends with an unconditional break.
     *
     * The break must be the final statement in the body.
     * If so, the 'repeat N times' can simply becomes 'repeat 1 times',
     * and from there the LoopUnroller takes charge.
     */
    bool endsWithUnconditionalBreak(const std::vector<std::unique_ptr<Stmt>> &body);

    /**
     * @brief Check if a class statement has no side effects.
     *
     * A class is considered side-effect free if it only contains methods
     * and no I/O operations, no file access, no network calls, and no global mutation.
     */
    bool hasNoSideEffects(const CreateClassStmt *stmt);

    /**
     * @brief Check if a method call expression has no side effects.
     */
    bool hasNoSideEffects(const MethodCallExpr *expr);

    /**
     * @brief Optimizes every statement in a sequential block.
     *
     * Statements may be removed, replaced, or left unchanged.
     * Statements following a control-flow terminator are discarded.
     */
    void optimizeBlock(std::vector<std::unique_ptr<Stmt>> &stmts, const std::unordered_set<std::string> &usedVars, const std::unordered_set<std::string> &usedClasses);
    void optimizeBlock(std::vector<std::shared_ptr<Stmt>> &stmts, const std::unordered_set<std::string> &usedVars, const std::unordered_set<std::string> &usedClasses);

    /**
     * @brief Optimizes a single statement.
     *
     * The statement may be preserved, removed, or replaced by one or more
     * optimized statements.
     */
    std::vector<std::unique_ptr<Stmt>> optimizeStatement(
            std::unique_ptr<Stmt> statement, const std::unordered_set<std::string> &usedVars, const std::unordered_set<std::string> &usedClasses);

    std::vector<std::shared_ptr<Stmt>> optimizeStatement(
            std::shared_ptr<Stmt> statement, const std::unordered_set<std::string> &usedVars, const std::unordered_set<std::string> &usedClasses);
};
