#include "deterministic_loop_optimizer.hpp"

#include <cmath>

#include "ast/stmt/create_var_stmt.hpp"
#include "ast/stmt/update_var_stmt.hpp"
#include "ast/stmt/assign_stmt.hpp"
#include "ast/stmt/if_stmt.hpp"
#include "ast/stmt/repeat_while_stmt.hpp"
#include "ast/stmt/repeat_times_stmt.hpp"
#include "ast/stmt/repeat_forever_stmt.hpp"
#include "ast/stmt/for_each_stmt.hpp"
#include "ast/stmt/create_class_stmt.hpp"
#include "ast/stmt/create_method_stmt.hpp"
#include "ast/stmt/try_stmt.hpp"
#include "ast/stmt/break_stmt.hpp"
#include "ast/stmt/skip_stmt.hpp"
#include "ast/stmt/return_stmt.hpp"
#include "ast/stmt/resist_stmt.hpp"

#include "ast/expr/literal_expr.hpp"
#include "ast/expr/variable_expr.hpp"
#include "ast/expr/binary_expr.hpp"

#include "types/known_types.hpp"

void DeterministicLoopOptimizer::run(std::vector<std::unique_ptr<Stmt>> &program) { optimizeBlock(program); }

void DeterministicLoopOptimizer::optimizeBlock(std::vector<std::unique_ptr<Stmt>> &stmts)
{
    for (size_t i = 0; i < stmts.size(); ++i)
    {
        if (!stmts[i])
            continue;

        if (auto repeatWhile = dynamic_cast<RepeatWhileStmt *>(stmts[i].get()))
        {
            auto replacement = tryOptimizeLoop(*repeatWhile, stmts, i);
            if (replacement)
            {
                stmts[i] = std::move(replacement);
                continue;
            }

            // --------------------------------------------------------------
            // The loop could not be reduced to a closed form, so analyze its
            // body separately, still looking for nested optimizable loops.
            // --------------------------------------------------------------
            optimizeBlock(repeatWhile->body);
            continue;
        }

        if (auto ifStmt = dynamic_cast<IfStmt *>(stmts[i].get()))
        {
            optimizeBlock(ifStmt->thenBranch);
            optimizeBlock(ifStmt->otherwiseBranch);
            continue;
        }

        if (auto repeatTimes = dynamic_cast<RepeatTimesStmt *>(stmts[i].get()))
        {
            optimizeBlock(repeatTimes->body);
            continue;
        }

        if (auto repeatForever = dynamic_cast<RepeatForeverStmt *>(stmts[i].get()))
        {
            optimizeBlock(repeatForever->body);
            continue;
        }

        if (auto forEach = dynamic_cast<ForEachStmt *>(stmts[i].get()))
        {
            optimizeBlock(forEach->body);
            continue;
        }

        if (auto createClass = dynamic_cast<CreateClassStmt *>(stmts[i].get()))
        {
            optimizeBlock(createClass->body);
            continue;
        }

        if (auto createMethod = dynamic_cast<CreateMethodStmt *>(stmts[i].get()))
        {
            optimizeBlock(createMethod->body);
            continue;
        }

        if (auto tryStmt = dynamic_cast<TryStmt *>(stmts[i].get()))
        {
            optimizeBlock(tryStmt->tryBody);

            for (auto &[type, body] : tryStmt->catchClauses)
                optimizeBlock(body);

            optimizeBlock(tryStmt->alwaysBody);
            continue;
        }
    }
}

std::optional<Value> DeterministicLoopOptimizer::findVariableInitialValue(
    const std::vector<std::unique_ptr<Stmt>> &block, size_t loopIndex, const VariableAddress &varAddress) const
{
    // ------------------------------------------------------------------
    // Scan the enclosing block backwards from the loop looking for the
    // most recent definition of the loop variable. The variable is
    // identified by its resolved VariableAddress, which was fixed at parse
    // time and never changes; the name is only kept for debugging. Because
    // constant propagation and folding ran first, that definition holds a
    // plain LiteralExpr when its value is still known.
    // ------------------------------------------------------------------
    for (size_t j = loopIndex; j-- > 0;)
    {
        const auto &s = block[j];
        if (!s)
            continue;

        if (auto create = dynamic_cast<const CreateVarStmt *>(s.get()))
        {
            if (varAddress == create->address)
            {
                if (create->value)
                    if (auto lit = dynamic_cast<const LiteralExpr *>(create->value.get()))
                        return lit->value;

                return std::nullopt;
            }
            continue;
        }

        if (auto update = dynamic_cast<const UpdateVarStmt *>(s.get()))
        {
            if (varAddress == update->address)
            {
                if (update->value)
                    if (auto lit = dynamic_cast<const LiteralExpr *>(update->value.get()))
                        return lit->value;

                return std::nullopt;
            }
            continue;
        }

        if (auto assign = dynamic_cast<const AssignStmt *>(s.get()))
        {
            if (auto var = dynamic_cast<const VariableExpr *>(assign->target.get()))
            {
                if (varAddress == var->address)
                {
                    if (assign->value)
                        if (auto lit = dynamic_cast<const LiteralExpr *>(assign->value.get()))
                            return lit->value;

                    return std::nullopt;
                }
            }
            continue;
        }

        // -------------------------------------------------------------
        // Any other statement (if, loop, try, method call, ...) could
        // modify the variable before we reach the loop, so the value can
        // no longer be trusted. Stop scanning.
        // -------------------------------------------------------------
        return std::nullopt;
    }

    return std::nullopt;
}

std::unique_ptr<Stmt> DeterministicLoopOptimizer::tryOptimizeLoop(
    const RepeatWhileStmt &loop, const std::vector<std::unique_ptr<Stmt>> &block, size_t loopIndex)
{
    // ------------------------------------------------------------------
    // 1. Analyze the loop condition (Condition must be side-effect free)
    // ------------------------------------------------------------------
    if (!loop.condition)
        return nullptr;

    auto binaryCond = dynamic_cast<const BinaryExpr *>(loop.condition.get());
    if (!binaryCond)
        return nullptr;

    std::string varName;
    VariableAddress varAddress;
    TokenType opType = binaryCond->op.type;
    const Expr *limitExpr = nullptr;

    if (auto leftVar = dynamic_cast<const VariableExpr *>(binaryCond->left.get()))
    {
        varName = leftVar->name;
        varAddress = leftVar->address;
        limitExpr = binaryCond->right.get();
    }
    else if (auto rightVar = dynamic_cast<const VariableExpr *>(binaryCond->right.get()))
    {
        varName = rightVar->name;
        varAddress = rightVar->address;
        limitExpr = binaryCond->left.get();

        // --------------------------------------------------------------------
        // Normalize the comparison so the loop variable is always on the left.
        //
        // This gives determineLoopFinalValue a single representation
        // regardless of how the original condition was written:
        //      10 < x  -> x > 10
        //      10 <= x -> x >= 10
        //      10 > x  -> x < 10
        //      10 >= x -> x <= 10
        //
        // Without normalization, determineLoopFinalValue would need to handle
        // both operand orders separately.
        // --------------------------------------------------------------------
        switch (opType)
        {
        case TokenType::LESS:
            opType = TokenType::GREATER;
            break;

        case TokenType::LESS_EQUAL:
            opType = TokenType::GREATER_EQUAL;
            break;

        case TokenType::GREATER:
            opType = TokenType::LESS;
            break;

        case TokenType::GREATER_EQUAL:
            opType = TokenType::LESS_EQUAL;
            break;

        default:
            return nullptr;
        }
    }
    else
    {
        return nullptr;
    }

    if (opType != TokenType::LESS && opType != TokenType::LESS_EQUAL && opType != TokenType::GREATER &&
        opType != TokenType::GREATER_EQUAL && opType != TokenType::NOT_EQUAL)
    {
        return nullptr;
    }

    auto limit = dynamic_cast<const LiteralExpr *>(limitExpr);
    if (!limit || !limit->value.IS_NUMBER)
        return nullptr;

    // ---------------------
    // 2. Loop body analysis
    // ---------------------
    if (loop.body.empty())
        return nullptr;

    double totalDeltaDouble = 0.0;
    int64_t totalDeltaInt = 0;
    bool deltaIsDouble = false;
    bool varUpdated = false;

    // ---------------------------------------------------------------
    // 2. Analyze the loop body
    //
    // The body must contain only side-effect-free updates to the loop
    // variable. Every update must add or subtract a constant step.
    // Anything else could affect execution or make the final state
    // impossible to determine safely.
    // ---------------------------------------------------------------
    for (const auto &stmt : loop.body)
    {
        if (!stmt)
            return nullptr;

        // ---------------------------------------
        // We can't optimize it if the loop breaks
        // ---------------------------------------
        if (dynamic_cast<const BreakStmt *>(stmt.get()) || dynamic_cast<const SkipStmt *>(stmt.get()) ||
            dynamic_cast<const ReturnStmt *>(stmt.get()) || dynamic_cast<const ResistStmt *>(stmt.get()))
            return nullptr;

        const Expr *valueExpr = nullptr;
        VariableAddress updateAddress;

        if (auto update = dynamic_cast<const UpdateVarStmt *>(stmt.get()))
        {
            updateAddress = update->address;
            valueExpr = update->value.get();
        }
        else if (auto assign = dynamic_cast<const AssignStmt *>(stmt.get()))
        {
            if (auto variable = dynamic_cast<const VariableExpr *>(assign->target.get()))
            {
                updateAddress = variable->address;
                valueExpr = assign->value.get();
            }
            else
            {
                return nullptr;
            }
        }
        else
        {
            return nullptr;
        }

        if (varAddress != updateAddress)
            return nullptr;

        if (!valueExpr)
            return nullptr;

        auto binValue = dynamic_cast<const BinaryExpr *>(valueExpr);
        if (!binValue)
            return nullptr;

        const Expr *stepExpr = nullptr;
        bool isSubtraction = false;

        if (binValue->op.type == TokenType::PLUS)
        {
            if (auto leftVar = dynamic_cast<const VariableExpr *>(binValue->left.get());
                leftVar && varAddress == leftVar->address)
            {
                stepExpr = binValue->right.get();
            }
            else if (
                auto rightVar = dynamic_cast<const VariableExpr *>(binValue->right.get());
                rightVar && varAddress == rightVar->address)
            {
                stepExpr = binValue->left.get();
            }
            else
            {
                return nullptr;
            }
        }
        else if (binValue->op.type == TokenType::MINUS)
        {
            if (auto leftVar = dynamic_cast<const VariableExpr *>(binValue->left.get());
                leftVar && varAddress == leftVar->address)
            {
                stepExpr = binValue->right.get();
                isSubtraction = true;
            }
            else
            {
                return nullptr;
            }
        }
        else
        {
            return nullptr;
        }

        auto literal = dynamic_cast<const LiteralExpr *>(stepExpr);
        if (!literal || !literal->value.IS_NUMBER)
            return nullptr;

        Value stepVal = literal->value;

        if (stepVal.IS_DOUBLE)
        {
            deltaIsDouble = true;
            double d = stepVal.toNumber();
            if (isSubtraction)
                d = -d;
            totalDeltaDouble += d;
            totalDeltaInt += static_cast<int64_t>(d);
        }
        else
        {
            int64_t d = stepVal.toInt();
            if (isSubtraction)
                d = -d;
            totalDeltaInt += d;
            totalDeltaDouble += static_cast<double>(d);
        }

        varUpdated = true;
    }

    if (!varUpdated)
        return nullptr;

    // -----------------------------------------------------------
    // 3. Resolve initial variable value from previous statements.
    // -----------------------------------------------------------
    auto startValue = findVariableInitialValue(block, loopIndex, varAddress);
    if (!startValue.has_value() || !startValue->IS_NUMBER)
        return nullptr;

    const Value &startVal = *startValue;

    // -------------------------------
    // 4. Resolve the loop limit value
    // -------------------------------
    auto limitLiteral = dynamic_cast<const LiteralExpr *>(limitExpr);
    if (!limitLiteral || !limitLiteral->value.IS_NUMBER)
        return nullptr;

    auto limitVal = limitLiteral->value;

    bool isDouble = startVal.IS_DOUBLE || limitVal.IS_DOUBLE || deltaIsDouble;

    // ---------------------------------------------------------------
    // 5. Mathematical derivation
    //
    // Instead of executing the loop, calculate how many iterations
    // are required for the condition to become false and derive the
    // variable's final value directly.
    // ---------------------------------------------------------------
    if (isDouble)
    {
        double initialValue = startVal.toNumber();
        double limit = limitVal.toNumber();
        double delta = totalDeltaDouble;

        double finalValue = initialValue;
        bool canReplaceLoopWithConstantNumber = determineLoopFinalValue(initialValue, limit, delta, opType, finalValue);
        if (!canReplaceLoopWithConstantNumber)
            return nullptr;

        Value resultValue(finalValue);
        return std::make_unique<UpdateVarStmt>(
            varName, varAddress, std::make_unique<LiteralExpr>(resultValue, KnownTypes::DOUBLE));
    }
    else
    {
        int64_t initialValue = startVal.toInt();
        int64_t limit = limitVal.toInt();
        int64_t delta = totalDeltaInt;

        int64_t finalValue = initialValue;
        bool canReplaceLoopWithConstantNumber = determineLoopFinalValue(initialValue, limit, delta, opType, finalValue);
        if (!canReplaceLoopWithConstantNumber)
            return nullptr;

        int finalInt = static_cast<int>(finalValue);
        Value resultValue(finalInt);
        return std::make_unique<UpdateVarStmt>(
            varName, varAddress, std::make_unique<LiteralExpr>(resultValue, KnownTypes::INT));
    }
}

bool DeterministicLoopOptimizer::determineLoopFinalValue(
    int64_t initialValue, int64_t limit, int64_t delta, TokenType opType, int64_t &finalValue)
{
    // --------------------------------------------------------------------
    // initialValue = initial value
    // limit = loop limit
    // delta = value added to the loop variable on each iteration
    //
    // The function returns false when the loop cannot be proven to
    // terminate. In that case the optimizer must leave the loop unchanged.
    // --------------------------------------------------------------------

    // ------------------------------------------------------
    // A variable that never changes can only stop the loop
    // if the condition is already false when the loop starts
    // ------------------------------------------------------
    if (delta == 0)
    {
        bool cond = false;
        if (opType == TokenType::LESS)
            cond = initialValue < limit;

        else if (opType == TokenType::LESS_EQUAL)
            cond = initialValue <= limit;

        else if (opType == TokenType::GREATER)
            cond = initialValue > limit;

        else if (opType == TokenType::GREATER_EQUAL)
            cond = initialValue >= limit;

        else if (opType == TokenType::NOT_EQUAL)
            cond = initialValue != limit;

        if (cond)
            return false; // Infinite loop

        finalValue = initialValue;
        return true;
    }

    if (delta > 0)
    {
        if (opType == TokenType::LESS)
        {
            if (initialValue >= limit)
            {
                finalValue = initialValue;
                return true;
            }
            int64_t diff = limit - initialValue;
            int64_t iterations = (diff + delta - 1) / delta;
            finalValue = initialValue + iterations * delta;
            return true;
        }
        else if (opType == TokenType::LESS_EQUAL)
        {
            if (initialValue > limit)
            {
                finalValue = initialValue;
                return true;
            }
            int64_t diff = limit - initialValue;
            int64_t iterations = (diff / delta) + 1;
            finalValue = initialValue + iterations * delta;
            return true;
        }
        else if (opType == TokenType::NOT_EQUAL)
        {
            if (initialValue == limit)
            {
                finalValue = initialValue;
                return true;
            }
            if (initialValue < limit && ((limit - initialValue) % delta == 0))
            {
                finalValue = limit;
                return true;
            }
            return false; // Infinite loop. The step never reaches the limit, so the loop never terminates.
        }
        else // GREATER or GREATER_EQUAL
        {
            bool cond = (opType == TokenType::GREATER) ? (initialValue > limit) : (initialValue >= limit);
            if (cond)
                return false; // Infinite loop

            finalValue = initialValue;
            return true;
        }
    }
    else // delta < 0
    {
        // -------------------------------------------------------------
        // Since delta is negative, make it positive so we can
        // calculate the number of iterations needed to reach the limit.
        // -------------------------------------------------------------
        delta = -delta;

        if (opType == TokenType::GREATER)
        {
            if (initialValue <= limit)
            {
                finalValue = initialValue;
                return true;
            }
            int64_t diff = initialValue - limit;
            int64_t iterations = (diff + delta - 1) / delta;
            finalValue = initialValue - iterations * delta;
            return true;
        }
        else if (opType == TokenType::GREATER_EQUAL)
        {
            if (initialValue < limit)
            {
                finalValue = initialValue;
                return true;
            }
            int64_t diff = initialValue - limit;
            int64_t iterations = (diff / delta) + 1;
            finalValue = initialValue - iterations * delta;
            return true;
        }
        else if (opType == TokenType::NOT_EQUAL)
        {
            if (initialValue == limit)
            {
                finalValue = initialValue;
                return true;
            }
            if (initialValue > limit && ((initialValue - limit) % delta == 0))
            {
                finalValue = limit;
                return true;
            }
            return false; // Infinite loop. The step never reaches the limit, so the loop never terminates.
        }
        else // LESS or LESS_EQUAL
        {
            bool cond = (opType == TokenType::LESS) ? (initialValue < limit) : (initialValue <= limit);
            if (cond)
                return false; // Infinite loop

            finalValue = initialValue;
            return true;
        }
    }
}

bool DeterministicLoopOptimizer::determineLoopFinalValue(
    double initialValue, double limit, double delta, TokenType opType, double &finalValue)
{
    // ------------------------------------------------------
    // A variable that never changes can only stop the loop
    // if the condition is already false when the loop starts
    // ------------------------------------------------------
    if (delta == 0.0)
    {
        bool cond = false;
        if (opType == TokenType::LESS)
            cond = initialValue < limit;

        else if (opType == TokenType::LESS_EQUAL)
            cond = initialValue <= limit;

        else if (opType == TokenType::GREATER)
            cond = initialValue > limit;

        else if (opType == TokenType::GREATER_EQUAL)
            cond = initialValue >= limit;

        else if (opType == TokenType::NOT_EQUAL)
            cond = initialValue != limit;

        if (cond)
            return false;

        finalValue = initialValue;
        return true;
    }

    if (delta > 0.0)
    {
        if (opType == TokenType::LESS)
        {
            if (initialValue >= limit)
            {
                finalValue = initialValue;
                return true;
            }
            double iterations = std::ceil((limit - initialValue) / delta);
            finalValue = initialValue + iterations * delta;
            return true;
        }
        else if (opType == TokenType::LESS_EQUAL)
        {
            if (initialValue > limit)
            {
                finalValue = initialValue;
                return true;
            }
            double iterations = std::floor((limit - initialValue) / delta) + 1.0;
            finalValue = initialValue + iterations * delta;
            return true;
        }
        else if (opType == TokenType::GREATER || opType == TokenType::GREATER_EQUAL)
        {
            bool cond = (opType == TokenType::GREATER) ? (initialValue > limit) : (initialValue >= limit);
            if (cond)
                return false;
            finalValue = initialValue;
            return true;
        }
        else if (opType == TokenType::NOT_EQUAL)
        {
            if (initialValue == limit)
            {
                finalValue = initialValue;
                return true;
            }
            return false;
        }
    }
    else // delta < 0.0
    {
        // -------------------------------------------------------------
        // Since delta is negative, make it positive so we can
        // calculate the number of iterations needed to reach the limit.
        // -------------------------------------------------------------
        delta = -delta;

        if (opType == TokenType::GREATER)
        {
            if (initialValue <= limit)
            {
                finalValue = initialValue;
                return true;
            }
            double iterations = std::ceil((initialValue - limit) / delta);
            finalValue = initialValue - iterations * delta;
            return true;
        }
        else if (opType == TokenType::GREATER_EQUAL)
        {
            if (initialValue < limit)
            {
                finalValue = initialValue;
                return true;
            }
            double iterations = std::floor((initialValue - limit) / delta) + 1.0;
            finalValue = initialValue - iterations * delta;
            return true;
        }
        else if (opType == TokenType::LESS || opType == TokenType::LESS_EQUAL)
        {
            bool cond = (opType == TokenType::LESS) ? (initialValue < limit) : (initialValue <= limit);
            if (cond)
                return false;
            finalValue = initialValue;
            return true;
        }
        else if (opType == TokenType::NOT_EQUAL)
        {
            if (initialValue == limit)
            {
                finalValue = initialValue;
                return true;
            }
            return false;
        }
    }
    return false;
}
