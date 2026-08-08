#include "dead_code_eliminator.hpp"

#include "ast/stmt/create_var_stmt.hpp"
#include "ast/stmt/update_var_stmt.hpp"
#include "ast/stmt/assign_stmt.hpp"
#include "ast/stmt/print_stmt.hpp"
#include "ast/stmt/return_stmt.hpp"
#include "ast/stmt/break_stmt.hpp"
#include "ast/stmt/skip_stmt.hpp"
#include "ast/stmt/if_stmt.hpp"
#include "ast/stmt/repeat_while_stmt.hpp"
#include "ast/stmt/repeat_times_stmt.hpp"
#include "ast/stmt/repeat_forever_stmt.hpp"
#include "ast/stmt/for_each_stmt.hpp"
#include "ast/stmt/create_class_stmt.hpp"
#include "ast/stmt/create_method_stmt.hpp"
#include "ast/stmt/try_stmt.hpp"
#include "ast/stmt/resist_stmt.hpp"
// #include "ast/stmt/create_var_with_ask_stmt.hpp"
// #include "ast/stmt/update_var_with_ask_stmt.hpp"

#include "ast/expr/literal_expr.hpp"
#include "ast/expr/variable_expr.hpp"
#include "ast/expr/grouping_expr.hpp"
#include "ast/expr/unary_expr.hpp"
#include "ast/expr/binary_expr.hpp"
#include "ast/expr/list_expr.hpp"
#include "ast/expr/dict_expr.hpp"
#include "ast/expr/conditional_expr.hpp"
#include "ast/expr/formatted_string_expr.hpp"
#include "ast/expr/method_call_expr.hpp"
#include "ast/expr/get_attr_expr.hpp"
#include "ast/expr/index_expr.hpp"
#include "ast/expr/ask_expr.hpp"
#include "ast/expr/create_instance_expr.hpp"
#include "ast/expr/parity_check_expr.hpp"

void DeadCodeEliminator::run(std::vector<std::unique_ptr<Stmt>> &program)
{
    bool changed = true;
    int maxPasses = 10;

    // --------------------------------------
    // Removing one dead statement may expose
    // additional dead code.
    //
    // Example:
    //
    //     create a = 1
    //     create b = a
    //     say(0)
    //
    // Pass 1 removes b.
    // Pass 2 removes a.
    //
    // That is why we iterate more than once.
    // --------------------------------------
    while (changed && maxPasses-- > 0)
    {
        std::unordered_set<std::string> usedVars;
        std::unordered_set<std::string> usedClasses;

        for (const auto &stmt : program)
        {
            if (stmt)
            {
                collectReferencedVariables(*stmt, usedVars);
                collectReferencedClasses(*stmt, usedClasses);
            }
        }

        size_t prevSize = program.size();
        optimizeBlock(program, usedVars, usedClasses);
        changed = (program.size() != prevSize);
    }
}

void DeadCodeEliminator::collectReferencedVariables(const Stmt &statement, std::unordered_set<std::string> &usedVars)
{
    if (auto create = dynamic_cast<const CreateVarStmt *>(&statement))
    {
        if (create->value)
            collectReferencedVariables(create->value.get(), usedVars);

        return;
    }

    if (auto update = dynamic_cast<const UpdateVarStmt *>(&statement))
    {
        if (update->value)
            collectReferencedVariables(update->value.get(), usedVars);

        return;
    }

    if (auto assign = dynamic_cast<const AssignStmt *>(&statement))
    {
        if (assign->target)
        {
            if (!dynamic_cast<const VariableExpr *>(assign->target.get()))
            {
                collectReferencedVariables(assign->target.get(), usedVars);
            }
        }
        if (assign->value)
            collectReferencedVariables(assign->value.get(), usedVars);

        return;
    }

    if (auto printStmt = dynamic_cast<const PrintStmt *>(&statement))
    {
        if (printStmt->message)
            collectReferencedVariables(printStmt->message.get(), usedVars);

        return;
    }

    if (auto returnStmt = dynamic_cast<const ReturnStmt *>(&statement))
    {
        if (returnStmt->value)
            collectReferencedVariables(returnStmt->value.get(), usedVars);

        return;
    }

    if (auto resist = dynamic_cast<const ResistStmt *>(&statement))
    {
        if (resist->messageExpr)
            collectReferencedVariables(resist->messageExpr.get(), usedVars);

        return;
    }

    if (auto ifStmt = dynamic_cast<const IfStmt *>(&statement))
    {
        if (ifStmt->condition)
            collectReferencedVariables(ifStmt->condition.get(), usedVars);

        for (const auto &child : ifStmt->thenBranch)
            if (child)
                collectReferencedVariables(*child, usedVars);

        for (const auto &child : ifStmt->otherwiseBranch)
            if (child)
                collectReferencedVariables(*child, usedVars);

        return;
    }

    if (auto repeatWhile = dynamic_cast<const RepeatWhileStmt *>(&statement))
    {
        if (repeatWhile->condition)
            collectReferencedVariables(repeatWhile->condition.get(), usedVars);

        for (const auto &child : repeatWhile->body)
            if (child)
                collectReferencedVariables(*child, usedVars);

        return;
    }

    if (auto repeatTimes = dynamic_cast<const RepeatTimesStmt *>(&statement))
    {
        if (repeatTimes->countExpr)
            collectReferencedVariables(repeatTimes->countExpr.get(), usedVars);

        for (const auto &child : repeatTimes->body)
            if (child)
                collectReferencedVariables(*child, usedVars);

        return;
    }

    if (auto repeatForever = dynamic_cast<const RepeatForeverStmt *>(&statement))
    {
        for (const auto &child : repeatForever->body)
            if (child)
                collectReferencedVariables(*child, usedVars);

        return;
    }

    if (auto forEach = dynamic_cast<const ForEachStmt *>(&statement))
    {
        if (forEach->iterable)
            collectReferencedVariables(forEach->iterable.get(), usedVars);

        for (const auto &child : forEach->body)
            if (child)
                collectReferencedVariables(*child, usedVars);

        return;
    }

    if (auto createClass = dynamic_cast<const CreateClassStmt *>(&statement))
    {
        for (const auto &child : createClass->body)
            if (child)
                collectReferencedVariables(*child, usedVars);

        return;
    }

    if (auto createMethod = dynamic_cast<const CreateMethodStmt *>(&statement))
    {
        for (const auto &child : createMethod->body)
            if (child)
                collectReferencedVariables(*child, usedVars);

        return;
    }

    if (auto tryStmt = dynamic_cast<const TryStmt *>(&statement))
    {
        for (const auto &child : tryStmt->tryBody)
            if (child)
                collectReferencedVariables(*child, usedVars);

        for (const auto &[type, body] : tryStmt->catchClauses)
            for (const auto &child : body)
                if (child)
                    collectReferencedVariables(*child, usedVars);

        for (const auto &child : tryStmt->alwaysBody)
            if (child)
                collectReferencedVariables(*child, usedVars);

        return;
    }
}

void DeadCodeEliminator::collectReferencedClasses(const Stmt &statement, std::unordered_set<std::string> &usedClasses)
{
    // FIXME: Rather than having to remember to add every new stmt supported in the language to
    // this method, we better add to the statement thelselves the collectReferencedClasses.

    if (auto createVar = dynamic_cast<const CreateVarStmt *>(&statement))
    {
        if (createVar->value)
            collectReferencedClasses(createVar->value.get(), usedClasses);

        return;
    }

    if (auto update = dynamic_cast<const UpdateVarStmt *>(&statement))
    {
        if (update->value)
            collectReferencedClasses(update->value.get(), usedClasses);

        return;
    }

    if (auto assign = dynamic_cast<const AssignStmt *>(&statement))
    {
        if (assign->target && !dynamic_cast<const VariableExpr *>(assign->target.get()))
            collectReferencedClasses(assign->target.get(), usedClasses);

        if (assign->value)
            collectReferencedClasses(assign->value.get(), usedClasses);

        return;
    }

    if (auto printStmt = dynamic_cast<const PrintStmt *>(&statement))
    {
        if (printStmt->message)
            collectReferencedClasses(printStmt->message.get(), usedClasses);

        return;
    }

    if (auto returnStmt = dynamic_cast<const ReturnStmt *>(&statement))
    {
        if (returnStmt->value)
            collectReferencedClasses(returnStmt->value.get(), usedClasses);

        return;
    }

    if (auto resist = dynamic_cast<const ResistStmt *>(&statement))
    {
        if (resist->messageExpr)
            collectReferencedClasses(resist->messageExpr.get(), usedClasses);

        return;
    }

    if (auto ifStmt = dynamic_cast<const IfStmt *>(&statement))
    {
        if (ifStmt->condition)
            collectReferencedClasses(ifStmt->condition.get(), usedClasses);

        for (const auto &child : ifStmt->thenBranch)
            if (child)
                collectReferencedClasses(*child, usedClasses);

        for (const auto &child : ifStmt->otherwiseBranch)
            if (child)
                collectReferencedClasses(*child, usedClasses);

        return;
    }

    if (auto repeatWhile = dynamic_cast<const RepeatWhileStmt *>(&statement))
    {
        if (repeatWhile->condition)
            collectReferencedClasses(repeatWhile->condition.get(), usedClasses);

        for (const auto &child : repeatWhile->body)
            if (child)
                collectReferencedClasses(*child, usedClasses);

        return;
    }

    if (auto repeatTimes = dynamic_cast<const RepeatTimesStmt *>(&statement))
    {
        if (repeatTimes->countExpr)
            collectReferencedClasses(repeatTimes->countExpr.get(), usedClasses);

        for (const auto &child : repeatTimes->body)
            if (child)
                collectReferencedClasses(*child, usedClasses);

        return;
    }

    if (auto repeatForever = dynamic_cast<const RepeatForeverStmt *>(&statement))
    {
        for (const auto &child : repeatForever->body)
            if (child)
                collectReferencedClasses(*child, usedClasses);

        return;
    }

    if (auto forEach = dynamic_cast<const ForEachStmt *>(&statement))
    {
        if (forEach->iterable)
            collectReferencedClasses(forEach->iterable.get(), usedClasses);

        for (const auto &child : forEach->body)
            if (child)
                collectReferencedClasses(*child, usedClasses);

        return;
    }

    if (auto createClass = dynamic_cast<const CreateClassStmt *>(&statement))
    {
        for (const auto &child : createClass->body)
            if (child)
                collectReferencedClasses(*child, usedClasses);

        return;
    }

    if (auto createMethod = dynamic_cast<const CreateMethodStmt *>(&statement))
    {
        for (const auto &child : createMethod->body)
            if (child)
                collectReferencedClasses(*child, usedClasses);

        return;
    }

    if (auto tryStmt = dynamic_cast<const TryStmt *>(&statement))
    {
        for (const auto &child : tryStmt->tryBody)
            if (child)
                collectReferencedClasses(*child, usedClasses);

        for (const auto &[type, body] : tryStmt->catchClauses)
            for (const auto &child : body)
                if (child)
                    collectReferencedClasses(*child, usedClasses);

        for (const auto &child : tryStmt->alwaysBody)
            if (child)
                collectReferencedClasses(*child, usedClasses);

        return;
    }
}

void DeadCodeEliminator::collectReferencedClasses(const Expr *expression, std::unordered_set<std::string> &usedClasses)
{
    if (!expression)
        return;

    if (auto varExpr = dynamic_cast<const VariableExpr *>(expression))
    {
        usedClasses.insert(varExpr->name);
        return;
    }

    if (auto grouping = dynamic_cast<const GroupingExpr *>(expression))
    {
        collectReferencedClasses(grouping->expression.get(), usedClasses);
        return;
    }

    if (auto unary = dynamic_cast<const UnaryExpr *>(expression))
    {
        collectReferencedClasses(unary->right.get(), usedClasses);
        return;
    }

    if (auto binary = dynamic_cast<const BinaryExpr *>(expression))
    {
        collectReferencedClasses(binary->left.get(), usedClasses);
        collectReferencedClasses(binary->right.get(), usedClasses);
        return;
    }

    if (auto listExpr = dynamic_cast<const ListExpr *>(expression))
    {
        for (const auto &el : listExpr->elements)
            collectReferencedClasses(el.get(), usedClasses);

        return;
    }

    if (auto dictExpr = dynamic_cast<const DictExpr *>(expression))
    {
        for (const auto &[k, v] : dictExpr->entries)
        {
            collectReferencedClasses(k.get(), usedClasses);
            collectReferencedClasses(v.get(), usedClasses);
        }

        return;
    }

    if (auto condExpr = dynamic_cast<const ConditionalExpr *>(expression))
    {
        collectReferencedClasses(condExpr->condition.get(), usedClasses);
        collectReferencedClasses(condExpr->thenBranch.get(), usedClasses);
        collectReferencedClasses(condExpr->elseBranch.get(), usedClasses);
        return;
    }

    if (auto fmtString = dynamic_cast<const FormattedStringExpr *>(expression))
    {
        for (const auto &expr : fmtString->expressions)
            collectReferencedClasses(expr.get(), usedClasses);

        return;
    }

    if (auto methodCall = dynamic_cast<const MethodCallExpr *>(expression))
    {
        collectReferencedClasses(methodCall->object.get(), usedClasses);
        for (const auto &arg : methodCall->args)
            collectReferencedClasses(arg.get(), usedClasses);

        return;
    }

    if (auto getAttr = dynamic_cast<const GetAttributeExpr *>(expression))
    {
        collectReferencedClasses(getAttr->object.get(), usedClasses);
        return;
    }

    if (auto indexExpr = dynamic_cast<const IndexExpr *>(expression))
    {
        collectReferencedClasses(indexExpr->collection.get(), usedClasses);
        collectReferencedClasses(indexExpr->index.get(), usedClasses);
        return;
    }

    if (auto createInst = dynamic_cast<const CreateInstanceExpr *>(expression))
    {
        usedClasses.insert(createInst->name);
        if (createInst->constructorArgs)
            collectReferencedClasses(createInst->constructorArgs.get(), usedClasses);

        return;
    }

    if (auto askExpr = dynamic_cast<const AskExpr *>(expression))
    {
        collectReferencedClasses(askExpr->prompt.get(), usedClasses);
        return;
    }

    if (auto parity = dynamic_cast<const ParityCheckExpr *>(expression))
    {
        collectReferencedClasses(parity->target.get(), usedClasses);
        return;
    }
}

void DeadCodeEliminator::collectReferencedVariables(const Expr *expression, std::unordered_set<std::string> &usedVars)
{
    if (!expression)
        return;

    //--------------------------------------------------------------
    // This is the heart of the method.
    // If it is a VariableExpr, then adds the expression to usedVars
    //--------------------------------------------------------------
    if (auto varExpr = dynamic_cast<const VariableExpr *>(expression))
    {
        usedVars.insert(varExpr->name);
        return;
    }

    if (auto grouping = dynamic_cast<const GroupingExpr *>(expression))
    {
        collectReferencedVariables(grouping->expression.get(), usedVars);
        return;
    }

    if (auto unary = dynamic_cast<const UnaryExpr *>(expression))
    {
        collectReferencedVariables(unary->right.get(), usedVars);
        return;
    }

    if (auto binary = dynamic_cast<const BinaryExpr *>(expression))
    {
        collectReferencedVariables(binary->left.get(), usedVars);
        collectReferencedVariables(binary->right.get(), usedVars);
        return;
    }

    if (auto listExpr = dynamic_cast<const ListExpr *>(expression))
    {
        for (const auto &el : listExpr->elements)
            collectReferencedVariables(el.get(), usedVars);

        return;
    }

    if (auto dictExpr = dynamic_cast<const DictExpr *>(expression))
    {
        for (const auto &[key, value] : dictExpr->entries)
        {
            collectReferencedVariables(key.get(), usedVars);
            collectReferencedVariables(value.get(), usedVars);
        }
        return;
    }

    if (auto condExpr = dynamic_cast<const ConditionalExpr *>(expression))
    {
        collectReferencedVariables(condExpr->condition.get(), usedVars);
        collectReferencedVariables(condExpr->thenBranch.get(), usedVars);
        collectReferencedVariables(condExpr->elseBranch.get(), usedVars);
        return;
    }

    if (auto methodCall = dynamic_cast<const MethodCallExpr *>(expression))
    {
        if (methodCall->object)
            collectReferencedVariables(methodCall->object.get(), usedVars);

        for (const auto &arg : methodCall->args)
            collectReferencedVariables(arg.get(), usedVars);

        return;
    }

    if (auto getAttr = dynamic_cast<const GetAttributeExpr *>(expression))
    {
        if (getAttr->object)
            collectReferencedVariables(getAttr->object.get(), usedVars);

        return;
    }

    if (auto indexExpr = dynamic_cast<const IndexExpr *>(expression))
    {
        if (indexExpr->collection)
            collectReferencedVariables(indexExpr->collection.get(), usedVars);

        if (indexExpr->index)
            collectReferencedVariables(indexExpr->index.get(), usedVars);

        return;
    }

    if (auto fmtString = dynamic_cast<const FormattedStringExpr *>(expression))
    {
        for (const auto &expr : fmtString->expressions)
            collectReferencedVariables(expr.get(), usedVars);

        return;
    }

    if (auto askExpr = dynamic_cast<const AskExpr *>(expression))
    {
        if (askExpr->prompt)
            collectReferencedVariables(askExpr->prompt.get(), usedVars);

        return;
    }

    if (auto createInst = dynamic_cast<const CreateInstanceExpr *>(expression))
    {
        if (createInst->constructorArgs)
            collectReferencedVariables(createInst->constructorArgs.get(), usedVars);

        return;
    }

    if (auto parity = dynamic_cast<const ParityCheckExpr *>(expression))
    {
        if (parity->target)
            collectReferencedVariables(parity->target.get(), usedVars);

        return;
    }
}

bool DeadCodeEliminator::hasNoSideEffects(const Expr *expression)
{
    if (!expression)
        return true;

    // -------------------------------------------------------------------
    // Reading a literal or a variable has no observable side effects.
    // Evaluating them cannot modify program state, perform I/O, or throw,
    // so they are safe to eliminate when their value is unused.
    // -------------------------------------------------------------------
    if (dynamic_cast<const LiteralExpr *>(expression) || dynamic_cast<const VariableExpr *>(expression))
    {
        return true;
    }

    if (auto grouping = dynamic_cast<const GroupingExpr *>(expression))
    {
        return hasNoSideEffects(grouping->expression.get());
    }

    if (auto unary = dynamic_cast<const UnaryExpr *>(expression))
    {
        return hasNoSideEffects(unary->right.get());
    }

    if (auto binary = dynamic_cast<const BinaryExpr *>(expression))
    {
        return hasNoSideEffects(binary->left.get()) && hasNoSideEffects(binary->right.get());
    }

    if (auto listExpr = dynamic_cast<const ListExpr *>(expression))
    {
        for (const auto &el : listExpr->elements)
            if (!hasNoSideEffects(el.get()))
                return false;

        return true;
    }

    if (auto dictExpr = dynamic_cast<const DictExpr *>(expression))
    {
        for (const auto &[k, v] : dictExpr->entries)
            if (!hasNoSideEffects(k.get()) || !hasNoSideEffects(v.get()))
                return false;

        return true;
    }

    if (auto condExpr = dynamic_cast<const ConditionalExpr *>(expression))
    {
        return hasNoSideEffects(condExpr->condition.get()) && hasNoSideEffects(condExpr->thenBranch.get()) &&
               hasNoSideEffects(condExpr->elseBranch.get());
    }

    if (auto getAttr = dynamic_cast<const GetAttributeExpr *>(expression))
    {
        return hasNoSideEffects(getAttr->object.get());
    }

    if (auto indexExpr = dynamic_cast<const IndexExpr *>(expression))
    {
        return hasNoSideEffects(indexExpr->collection.get()) && hasNoSideEffects(indexExpr->index.get());
    }

    if (auto fmtString = dynamic_cast<const FormattedStringExpr *>(expression))
    {
        for (const auto &expr : fmtString->expressions)
            if (!hasNoSideEffects(expr.get()))
                return false;

        return true;
    }

    if (auto parity = dynamic_cast<const ParityCheckExpr *>(expression))
    {
        return hasNoSideEffects(parity->target.get());
    }

    if (auto createInst = dynamic_cast<const CreateInstanceExpr *>(expression))
    {
        if (createInst->constructorArgs)
            return hasNoSideEffects(createInst->constructorArgs.get());

        return true;
    }

    return false;
}

bool DeadCodeEliminator::hasNoSideEffects(const CreateClassStmt *stmt)
{
    for (const auto &method : stmt->body)
    {
        if (!method)
            continue;

        if (auto createMethod = dynamic_cast<const CreateMethodStmt *>(method.get()))
        {
            for (const auto &stmt : createMethod->body)
            {
                if (!stmt)
                    continue;

                if (auto returnStmt = dynamic_cast<const ReturnStmt *>(stmt.get()))
                {
                    if (returnStmt->value && !hasNoSideEffects(returnStmt->value.get()))
                        return false;
                }
                else if (auto printStmt = dynamic_cast<const PrintStmt *>(stmt.get()))
                {
                    if (printStmt->message && !hasNoSideEffects(printStmt->message.get()))
                        return false;
                }
                else if (auto assign = dynamic_cast<const AssignStmt *>(stmt.get()))
                {
                    if (assign->value && !hasNoSideEffects(assign->value.get()))
                        return false;
                }
                else if (auto update = dynamic_cast<const UpdateVarStmt *>(stmt.get()))
                {
                    if (update->value && !hasNoSideEffects(update->value.get()))
                        return false;
                }
                else if (auto methodCall = dynamic_cast<const MethodCallExpr *>(stmt.get()))
                {
                    if (!hasNoSideEffects(methodCall))
                        return false;
                }
                else if (auto ifStmt = dynamic_cast<const IfStmt *>(stmt.get()))
                {
                    if (ifStmt->condition && !hasNoSideEffects(ifStmt->condition.get()))
                        return false;

                    for (const auto &s : ifStmt->thenBranch)
                    {
                        if (s && !hasNoSideEffects(s.get()))
                            return false;
                    }

                    for (const auto &s : ifStmt->otherwiseBranch)
                    {
                        if (s && !hasNoSideEffects(s.get()))
                            return false;
                    }
                }
                else if (auto repeatWhile = dynamic_cast<const RepeatWhileStmt *>(stmt.get()))
                {
                    if (repeatWhile->condition && !hasNoSideEffects(repeatWhile->condition.get()))
                        return false;

                    for (const auto &s : repeatWhile->body)
                    {
                        if (s && !hasNoSideEffects(s.get()))
                            return false;
                    }
                }
                else if (auto repeatTimes = dynamic_cast<const RepeatTimesStmt *>(stmt.get()))
                {
                    if (repeatTimes->countExpr && !hasNoSideEffects(repeatTimes->countExpr.get()))
                        return false;

                    for (const auto &s : repeatTimes->body)
                    {
                        if (s && !hasNoSideEffects(s.get()))
                            return false;
                    }
                }
                else if (auto forEach = dynamic_cast<const ForEachStmt *>(stmt.get()))
                {
                    if (forEach->iterable && !hasNoSideEffects(forEach->iterable.get()))
                        return false;

                    for (const auto &s : forEach->body)
                    {
                        if (s && !hasNoSideEffects(s.get()))
                            return false;
                    }
                }
                else
                {
                    return false;
                }
            }
        }
        else
        {
            return false;
        }
    }

    return true;
}

bool DeadCodeEliminator::hasNoSideEffects(const MethodCallExpr *expr)
{
    if (!hasNoSideEffects(expr->object.get()))
        return false;

    for (const auto &arg : expr->args)
    {
        if (!hasNoSideEffects(arg.get()))
            return false;
    }

    return true;
}

bool DeadCodeEliminator::hasNoSideEffects(const Stmt *stmt)
{
    if (!stmt)
        return true;

    if (auto returnStmt = dynamic_cast<const ReturnStmt *>(stmt))
    {
        if (returnStmt->value)
            return hasNoSideEffects(returnStmt->value.get());

        return true;
    }

    if (auto printStmt = dynamic_cast<const PrintStmt *>(stmt))
    {
        if (printStmt->message)
            return hasNoSideEffects(printStmt->message.get());

        return true;
    }

    if (auto assign = dynamic_cast<const AssignStmt *>(stmt))
    {
        if (assign->value)
            return hasNoSideEffects(assign->value.get());

        return true;
    }

    if (auto update = dynamic_cast<const UpdateVarStmt *>(stmt))
    {
        if (update->value)
            return hasNoSideEffects(update->value.get());

        return true;
    }

    if (auto methodCall = dynamic_cast<const MethodCallExpr *>(stmt))
    {
        return hasNoSideEffects(methodCall);
    }

    if (auto ifStmt = dynamic_cast<const IfStmt *>(stmt))
    {
        if (ifStmt->condition && !hasNoSideEffects(ifStmt->condition.get()))
            return false;

        for (const auto &s : ifStmt->thenBranch)
        {
            if (s && !hasNoSideEffects(s.get()))
                return false;
        }

        for (const auto &s : ifStmt->otherwiseBranch)
        {
            if (s && !hasNoSideEffects(s.get()))
                return false;
        }

        return true;
    }

    if (auto repeatWhile = dynamic_cast<const RepeatWhileStmt *>(stmt))
    {
        if (repeatWhile->condition && !hasNoSideEffects(repeatWhile->condition.get()))
            return false;

        for (const auto &s : repeatWhile->body)
        {
            if (s && !hasNoSideEffects(s.get()))
                return false;
        }

        return true;
    }

    if (auto repeatTimes = dynamic_cast<const RepeatTimesStmt *>(stmt))
    {
        if (repeatTimes->countExpr && !hasNoSideEffects(repeatTimes->countExpr.get()))
            return false;

        for (const auto &s : repeatTimes->body)
        {
            if (s && !hasNoSideEffects(s.get()))
                return false;
        }

        return true;
    }

    if (auto repeatForever = dynamic_cast<const RepeatForeverStmt *>(stmt))
    {
        for (const auto &s : repeatForever->body)
        {
            if (s && !hasNoSideEffects(s.get()))
                return false;
        }

        return true;
    }

    if (auto forEach = dynamic_cast<const ForEachStmt *>(stmt))
    {
        if (forEach->iterable && !hasNoSideEffects(forEach->iterable.get()))
            return false;

        for (const auto &s : forEach->body)
        {
            if (s && !hasNoSideEffects(s.get()))
                return false;
        }

        return true;
    }

    return false;
}

bool DeadCodeEliminator::canDiscard(const CreateVarStmt *stmt, const std::unordered_set<std::string> &usedVars)
{
    return !usedVars.contains(stmt->name) && hasNoSideEffects(stmt->value.get());
}

bool DeadCodeEliminator::canDiscard(const UpdateVarStmt *stmt, const std::unordered_set<std::string> &usedVars)
{
    return !usedVars.contains(stmt->name) && hasNoSideEffects(stmt->value.get());
}

bool DeadCodeEliminator::canDiscard(const AssignStmt *stmt, const std::unordered_set<std::string> &usedVars)
{
    auto variable = dynamic_cast<const VariableExpr *>(stmt->target.get());

    return variable && !usedVars.contains(variable->name) && hasNoSideEffects(stmt->value.get());
}

bool DeadCodeEliminator::canDiscard(const CreateClassStmt *stmt, const std::unordered_set<std::string> &usedClasses)
{
    return !usedClasses.contains(stmt->name) && hasNoSideEffects(stmt);
}

bool DeadCodeEliminator::isControlFlowTerminator(const Stmt &statement)
{
    return dynamic_cast<const ReturnStmt *>(&statement) != nullptr ||
           dynamic_cast<const BreakStmt *>(&statement) != nullptr ||
           dynamic_cast<const SkipStmt *>(&statement) != nullptr ||
           dynamic_cast<const ResistStmt *>(&statement) != nullptr;
}

void DeadCodeEliminator::optimizeBlock(
    std::vector<std::unique_ptr<Stmt>> &stmts,
    const std::unordered_set<std::string> &usedVars,
    const std::unordered_set<std::string> &usedClasses)
{
    std::vector<std::unique_ptr<Stmt>> result;
    result.reserve(stmts.size());

    for (auto &stmt : stmts)
    {
        if (!stmt)
            continue;

        bool isTerminator = isControlFlowTerminator(*stmt);

        std::vector<std::unique_ptr<Stmt>> replacements = optimizeStatement(std::move(stmt), usedVars, usedClasses);

        for (auto &replacement : replacements)
        {
            if (replacement)
            {
                result.push_back(std::move(replacement));
            }
        }

        // -------------------------------------
        // If we reached a terminator statement,
        // we break to remove what comes after.
        //
        // Code example:
        //
        //     say("before")
        //     return
        //     say("after")
        //
        // becomes:
        //
        //     say("before")
        //     return
        //
        // Nested blocks are optimized independently. For example:
        //
        //     if condition:
        //         say("before")
        //         return
        //         say("after")
        //     otherwise:
        //         say("before")
        //         return
        //         say("after")
        //     amen
        //
        //     say("outside")
        //
        // becomes:
        //
        //     if condition:
        //         say("before")
        //         return
        //     otherwise:
        //         say("before")
        //         return
        //     amen
        // -------------------------------------
        if (isTerminator)
        {
            break;
        }
    }

    stmts = std::move(result);
}

void DeadCodeEliminator::optimizeBlock(
    std::vector<std::shared_ptr<Stmt>> &stmts,
    const std::unordered_set<std::string> &usedVars,
    const std::unordered_set<std::string> &usedClasses)
{
    std::vector<std::shared_ptr<Stmt>> result;
    result.reserve(stmts.size());

    for (auto &stmt : stmts)
    {
        if (!stmt)
            continue;

        bool isTerminator = isControlFlowTerminator(*stmt);

        std::vector<std::shared_ptr<Stmt>> replacements = optimizeStatement(stmt, usedVars, usedClasses);

        for (auto &replacement : replacements)
        {
            if (replacement)
            {
                result.push_back(replacement);
            }
        }

        if (isTerminator)
        {
            break;
        }
    }

    stmts = std::move(result);
}

std::vector<std::unique_ptr<Stmt>> DeadCodeEliminator::optimizeStatement(
    std::unique_ptr<Stmt> stmt,
    const std::unordered_set<std::string> &usedVars,
    const std::unordered_set<std::string> &usedClasses)
{
    std::vector<std::unique_ptr<Stmt>> result;

    if (!stmt)
        return result;

    if (auto create = dynamic_cast<CreateVarStmt *>(stmt.get()))
    {
        if (canDiscard(create, usedVars))
        {
            return result;
        }
        result.push_back(std::move(stmt));
        return result;
    }

    if (auto update = dynamic_cast<UpdateVarStmt *>(stmt.get()))
    {
        if (canDiscard(update, usedVars))
        {
            return result;
        }
        result.push_back(std::move(stmt));
        return result;
    }

    if (auto assign = dynamic_cast<AssignStmt *>(stmt.get()))
    {
        if (canDiscard(assign, usedVars))
        {
            return result;
        }
        result.push_back(std::move(stmt));
        return result;
    }

    if (auto ifStmt = dynamic_cast<IfStmt *>(stmt.get()))
    {
        std::unordered_set<std::string> emptyClasses;
        optimizeBlock(ifStmt->thenBranch, usedVars, emptyClasses);
        optimizeBlock(ifStmt->otherwiseBranch, usedVars, emptyClasses);

        if (ifStmt->condition && ifStmt->condition->canEvaluateAtParseTime())
        {
            try
            {
                Value val = ifStmt->condition->evaluate(nullptr);
                if (val.AS_BOOLEAN)
                {
                    for (auto &s : ifStmt->thenBranch)
                    {
                        if (s)
                            result.push_back(std::move(s));
                    }
                    return result;
                }
                else
                {
                    for (auto &s : ifStmt->otherwiseBranch)
                    {
                        if (s)
                            result.push_back(std::move(s));
                    }
                    return result;
                }
            }
            catch (...)
            {
            }
        }

        if (ifStmt->thenBranch.empty() && ifStmt->otherwiseBranch.empty())
        {
            if (hasNoSideEffects(ifStmt->condition.get()))
            {
                return result;
            }
        }

        result.push_back(std::move(stmt));
        return result;
    }

    if (auto repeatWhile = dynamic_cast<RepeatWhileStmt *>(stmt.get()))
    {
        std::unordered_set<std::string> emptyClasses;
        optimizeBlock(repeatWhile->body, usedVars, emptyClasses);

        if (repeatWhile->condition && repeatWhile->condition->canEvaluateAtParseTime())
        {
            try
            {
                Value val = repeatWhile->condition->evaluate(nullptr);
                if (!val.AS_BOOLEAN)
                {
                    return result;
                }
            }
            catch (...)
            {
            }
        }

        if (repeatWhile->body.empty() && hasNoSideEffects(repeatWhile->condition.get()))
        {
            return result;
        }

        result.push_back(std::move(stmt));
        return result;
    }

    if (auto repeatTimes = dynamic_cast<RepeatTimesStmt *>(stmt.get()))
    {
        std::unordered_set<std::string> emptyClasses;
        optimizeBlock(repeatTimes->body, usedVars, emptyClasses);

        if (repeatTimes->countExpr && repeatTimes->countExpr->canEvaluateAtParseTime())
        {
            try
            {
                Value val = repeatTimes->countExpr->evaluate(nullptr);
                if (val.IS_NUMBER && val.toInt() <= 0)
                {
                    return result;
                }
            }
            catch (...)
            {
            }
        }

        if (repeatTimes->body.empty() && hasNoSideEffects(repeatTimes->countExpr.get()))
        {
            return result;
        }

        result.push_back(std::move(stmt));
        return result;
    }

    if (auto repeatForever = dynamic_cast<RepeatForeverStmt *>(stmt.get()))
    {
        std::unordered_set<std::string> emptyClasses;
        optimizeBlock(repeatForever->body, usedVars, emptyClasses);
        result.push_back(std::move(stmt));
        return result;
    }

    if (auto forEach = dynamic_cast<ForEachStmt *>(stmt.get()))
    {
        std::unordered_set<std::string> emptyClasses;
        optimizeBlock(forEach->body, usedVars, emptyClasses);
        result.push_back(std::move(stmt));
        return result;
    }

    if (auto createClass = dynamic_cast<CreateClassStmt *>(stmt.get()))
    {
        optimizeBlock(createClass->body, usedVars, usedClasses);

        if (canDiscard(createClass, usedClasses))
            return result;

        result.push_back(std::move(stmt));
        return result;
    }

    if (auto createMethod = dynamic_cast<CreateMethodStmt *>(stmt.get()))
    {
        std::unordered_set<std::string> emptyClasses;
        optimizeBlock(createMethod->body, usedVars, emptyClasses);
        result.push_back(std::move(stmt));
        return result;
    }

    if (auto tryStmt = dynamic_cast<TryStmt *>(stmt.get()))
    {
        std::unordered_set<std::string> emptyClasses;
        optimizeBlock(tryStmt->tryBody, usedVars, emptyClasses);
        for (auto &[type, catchBody] : tryStmt->catchClauses)
        {
            optimizeBlock(catchBody, usedVars, emptyClasses);
        }
        optimizeBlock(tryStmt->alwaysBody, usedVars, emptyClasses);
        result.push_back(std::move(stmt));
        return result;
    }

    result.push_back(std::move(stmt));
    return result;
}

std::vector<std::shared_ptr<Stmt>> DeadCodeEliminator::optimizeStatement(
    std::shared_ptr<Stmt> stmt,
    const std::unordered_set<std::string> &usedVars,
    const std::unordered_set<std::string> &usedClasses)
{
    std::vector<std::shared_ptr<Stmt>> result;

    if (!stmt)
        return result;

    if (auto create = dynamic_cast<CreateVarStmt *>(stmt.get()))
    {
        if (canDiscard(create, usedVars))
        {
            return result;
        }
        result.push_back(stmt);
        return result;
    }

    if (auto update = dynamic_cast<UpdateVarStmt *>(stmt.get()))
    {
        if (canDiscard(update, usedVars))
        {
            return result;
        }
        result.push_back(stmt);
        return result;
    }

    if (auto assign = dynamic_cast<AssignStmt *>(stmt.get()))
    {
        if (canDiscard(assign, usedVars))
        {
            return result;
        }
        result.push_back(stmt);
        return result;
    }

    if (auto ifStmt = dynamic_cast<IfStmt *>(stmt.get()))
    {
        std::unordered_set<std::string> emptyClasses;
        optimizeBlock(ifStmt->thenBranch, usedVars, emptyClasses);
        optimizeBlock(ifStmt->otherwiseBranch, usedVars, emptyClasses);

        if (ifStmt->condition && ifStmt->condition->canEvaluateAtParseTime())
        {
            try
            {
                Value val = ifStmt->condition->evaluate(nullptr);
                if (val.AS_BOOLEAN)
                {
                    for (auto &s : ifStmt->thenBranch)
                    {
                        if (s)
                            result.push_back(std::shared_ptr<Stmt>(std::move(s)));
                    }
                    return result;
                }
                else
                {
                    for (auto &s : ifStmt->otherwiseBranch)
                    {
                        if (s)
                            result.push_back(std::shared_ptr<Stmt>(std::move(s)));
                    }
                    return result;
                }
            }
            catch (...)
            {
            }
        }

        if (ifStmt->thenBranch.empty() && ifStmt->otherwiseBranch.empty())
        {
            if (hasNoSideEffects(ifStmt->condition.get()))
            {
                return result;
            }
        }

        result.push_back(stmt);
        return result;
    }

    if (auto repeatWhile = dynamic_cast<RepeatWhileStmt *>(stmt.get()))
    {
        std::unordered_set<std::string> emptyClasses;
        optimizeBlock(repeatWhile->body, usedVars, emptyClasses);

        if (repeatWhile->condition && repeatWhile->condition->canEvaluateAtParseTime())
        {
            try
            {
                Value val = repeatWhile->condition->evaluate(nullptr);
                if (!val.AS_BOOLEAN)
                {
                    return result;
                }
            }
            catch (...)
            {
            }
        }

        if (repeatWhile->body.empty() && hasNoSideEffects(repeatWhile->condition.get()))
        {
            return result;
        }

        result.push_back(stmt);
        return result;
    }

    if (auto repeatTimes = dynamic_cast<RepeatTimesStmt *>(stmt.get()))
    {
        std::unordered_set<std::string> emptyClasses;
        optimizeBlock(repeatTimes->body, usedVars, emptyClasses);

        if (repeatTimes->countExpr && repeatTimes->countExpr->canEvaluateAtParseTime())
        {
            try
            {
                Value val = repeatTimes->countExpr->evaluate(nullptr);
                if (val.IS_NUMBER && val.toInt() <= 0)
                {
                    return result;
                }
            }
            catch (...)
            {
            }
        }

        if (repeatTimes->body.empty() && hasNoSideEffects(repeatTimes->countExpr.get()))
        {
            return result;
        }

        result.push_back(stmt);
        return result;
    }

    if (auto repeatForever = dynamic_cast<RepeatForeverStmt *>(stmt.get()))
    {
        std::unordered_set<std::string> emptyClasses;
        optimizeBlock(repeatForever->body, usedVars, emptyClasses);
        result.push_back(stmt);
        return result;
    }

    if (auto forEach = dynamic_cast<ForEachStmt *>(stmt.get()))
    {
        std::unordered_set<std::string> emptyClasses;
        optimizeBlock(forEach->body, usedVars, emptyClasses);
        result.push_back(stmt);
        return result;
    }

    if (auto createClass = dynamic_cast<CreateClassStmt *>(stmt.get()))
    {
        optimizeBlock(createClass->body, usedVars, usedClasses);

        if (canDiscard(createClass, usedClasses))
        {
            return result;
        }

        result.push_back(stmt);
        return result;
    }

    if (auto createMethod = dynamic_cast<CreateMethodStmt *>(stmt.get()))
    {
        std::unordered_set<std::string> emptyClasses;
        optimizeBlock(createMethod->body, usedVars, emptyClasses);
        result.push_back(stmt);
        return result;
    }

    if (auto tryStmt = dynamic_cast<TryStmt *>(stmt.get()))
    {
        std::unordered_set<std::string> emptyClasses;
        optimizeBlock(tryStmt->tryBody, usedVars, emptyClasses);
        for (auto &[type, catchBody] : tryStmt->catchClauses)
        {
            optimizeBlock(catchBody, usedVars, emptyClasses);
        }
        optimizeBlock(tryStmt->alwaysBody, usedVars, emptyClasses);
        result.push_back(stmt);
        return result;
    }

    result.push_back(stmt);
    return result;
}
