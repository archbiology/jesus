#pragma once

#include <vector>
#include <memory>
#include <unordered_map>
#include <unordered_set>
#include <string>
#include <optional>

#include "ast/stmt/stmt.hpp"
#include "ast/stmt/repeat_while_stmt.hpp"
#include "ast/expr/expr.hpp"
#include "lexer/token_type.hpp"
#include "spirit/value.hpp"
#include "spirit/symbol_table.hpp"

/**
 * @brief Optimizes deterministic loops without side effects by mathematically
 * deriving their final state instead of executing each iteration.
 *
 * Specifically detects loops where:
 *   - The loop condition depends only on a variable being monotonically updated.
 *   - Each iteration applies a deterministic constant transformation to that variable.
 *   - Neither the condition nor the loop body has observable side effects.
 *   - The final value can be calculated directly while preserving the exact semantics
 *     of the original program (e.g. integer overflow, type, evaluation order).
 *
 * Example:
 *
 *     repeat while sum < 10000000:
 *         sum = sum + 6
 *     amen
 *
 * becomes:
 *
 *     sum = 10000002
 *
 * "For which of you, desiring to build a tower, does not first sit down and count the cost,
 * whether he has enough to complete it?"
 * — Luke 14:28
 */
class DeterministicLoopOptimizer
{
    using KnownVariables = std::unordered_map<std::string, Value>;

  public:
    /**
     * @brief
     *
     * DeterministicLoopOptimizer::run
     *   │
     *   ├── optimizeBlock
     *   │   ├── track known values
     *   │   ├── propagate state through branches
     *   │   └── isolate loop/exception state
     *   │
     *   ├── tryOptimizeLoop
     *   │   ├── prove condition is deterministic
     *   │   ├── prove body is a simple counter update
     *   │   ├── resolve initialValue/limit/step
     *   │   └── derive final state mathematically
     *   │
     *   ├── computeFinalStateInt
     *   ├── computeFinalStateDouble
     *   │
     *   └── safety helpers
     *       ├── referencesVariable
     *       ├── hasNoSideEffects
     *       └── collectModifiedVars
     */
    void run(std::vector<std::unique_ptr<Stmt>> &program);

  private:
    void optimizeBlock(std::vector<std::unique_ptr<Stmt>> &stmts);

    std::unique_ptr<Stmt> tryOptimizeLoop(
        const RepeatWhileStmt &loop, const std::vector<std::unique_ptr<Stmt>> &block, size_t loopIndex);

    std::optional<Value> findVariableInitialValue(
        const std::vector<std::unique_ptr<Stmt>> &block, size_t loopIndex, const VariableAddress &address) const;

    /**
     * @brief Determines the final value of a loop variable without executing the loop.
     *
     * Instead of executing `x = x + delta` N times,
     * calculate N and jump directly to the value after those N iterations.
     *
     * The loop is assumed to have this form:
     *
     *     while variable <op> limit
     *         variable = variable +- delta
     *
     * where `delta` is a positive constant. The optimizer converts subtraction
     * into a negative change internally.
     *
     * For increasing loops:
     *
     *     variable < limit:
     *         iterations = ceil((limit - initialValue) / delta)
     *         finalValue = initialValue + iterations * delta
     *
     *     variable <= limit:
     *         iterations = floor((limit - initialValue) / delta) + 1
     *         finalValue = initialValue + iterations * delta
     *
     * For decreasing loops:
     *
     *     variable > limit:
     *         iterations = ceil((initialValue - limit) / delta)
     *         finalValue = initialValue - iterations * delta
     *
     *     variable >= limit:
     *         iterations = floor((initialValue - limit) / delta) + 1
     *         finalValue = initialValue - iterations * delta
     *
     * For !=, the loop stops only when the variable reaches the limit exactly.
     * If a step would jump over the limit, the loop never stops:
     *
     *     0, 2, 4, 6  → limit 6  → stops
     *     0, 2, 4, 6  → limit 5  → never stops
     *
     * When it reaches the limit:
     *
     *     finalValue = limit
     *
     * If the condition is already false, the loop runs zero times and
     * finalValue remains initialValue.
     *
     * If delta is zero and the condition is true, the variable never changes,
     * so the loop never terminates.
     *
     * When finalVal can be deterministically determined, finalVal contains
     * the value the variable will have after the loop finishes.
     *
     * @return true if the loop can be replaced with a constant number
     * @return false if the loop can't be proven to terminate in a deterministic way.
     */
    bool determineLoopFinalValue(
        int64_t initialValue, int64_t limit, int64_t delta, TokenType opType, int64_t &finalVal);

    bool determineLoopFinalValue(double initialValue, double limit, double delta, TokenType opType, double &finalVal);
};
