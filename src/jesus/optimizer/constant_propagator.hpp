#pragma once

#include <vector>
#include <memory>
#include <unordered_set>
#include <unordered_map>
#include <string>

#include "ast/stmt/stmt.hpp"
#include "ast/expr/expr.hpp"
#include "ast/expr/literal_expr.hpp"

class CreateVarStmt;
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
    using Expression = std::unique_ptr<Expr>;

    using VariableName = std::string;
    using InstanceVariableName = std::string;
    using AttributeName = std::string;
    using ScopeId = uint32_t;
    using Slot = uint32_t;

    using AttributeValues = std::unordered_map<AttributeName, Expression>;

    using ConstVariables = std::unordered_map<VariableName, Expression>;
    using ClassAttributeValues = std::unordered_map<ScopeId, std::unordered_map<Slot, Expression>>;
    using ModifiedVariables = std::unordered_set<VariableName>;
    using DeclaredVariables = std::unordered_set<VariableName>;

    struct State
    {
        ModifiedVariables modifiedVars;
        DeclaredVariables declaredVars;
        ConstVariables constVars;
        ClassAttributeValues classAttributeValues;
    };

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
    void collectModifiedVars(const Stmt &statement, State &state);

    void collectModifiedVarsFromExpr(const Expr *expression, State &state);

    /**
     * @brief Collects the declared initial values of every class attribute in
     * the program.
     *
     * A reference to a class attribute (a VariableExpr declared in the class
     * scope, e.g. `return name` inside a method) carries the scopeId and
     * slot of the class attributes scope. When the attribute is never modified
     * its declared initial value is its constant value, so the reference can be
     * replaced by that literal.
     */
    void collectClassAttributeValues(const std::vector<std::unique_ptr<Stmt>> &program, State &state);

    void replaceConstWithLiteralInStatement(Stmt &statement, State &state);

    /**
     * @brief This is the heart of ConstantPropagator.
     *
     * If this expression is a variable, and I know its constant value,
     *    replace the variable by a copy of that constant.
     * Otherwise,
     *    leave the expression unchanged.
     */
    std::unique_ptr<Expr> replaceConstWithLiteralInExpression(std::unique_ptr<Expr> expression, State &state);

    void propagateConstructorArguments(const CreateVarStmt *create, State &state);

    std::unique_ptr<Expr> cloneExpr(const Expr &expr);
    std::unique_ptr<Expr> createLiteral(const Value &value);
};
