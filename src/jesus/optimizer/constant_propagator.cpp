#include "constant_propagator.hpp"

#include "ast/stmt/create_var_stmt.hpp"
#include "ast/stmt/update_var_stmt.hpp"
#include "ast/stmt/assign_stmt.hpp"
#include "ast/stmt/print_stmt.hpp"
#include "ast/stmt/return_stmt.hpp"
#include "ast/stmt/if_stmt.hpp"
#include "ast/stmt/repeat_while_stmt.hpp"
#include "ast/stmt/repeat_times_stmt.hpp"
#include "ast/stmt/repeat_forever_stmt.hpp"
#include "ast/stmt/for_each_stmt.hpp"
#include "ast/stmt/create_class_stmt.hpp"
#include "ast/stmt/create_method_stmt.hpp"
#include "ast/stmt/try_stmt.hpp"
#include "ast/stmt/resist_stmt.hpp"
#include "ast/stmt/create_var_with_ask_stmt.hpp"
#include "ast/stmt/update_var_with_ask_stmt.hpp"

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

#include "types/known_types.hpp"

void ConstantPropagator::run(std::vector<std::unique_ptr<Stmt>> &program)
{
    State state;

    // ----------------------------------------------------------------
    // Pass 1
    // ------
    // Walk the entire program and discover which variables may change.
    // Any variable that is reassigned, written by user input, modified
    // inside loops, or otherwise mutated is marked as non-constant.
    // ----------------------------------------------------------------
    for (const auto &stmt : program)
    {
        if (stmt)
            collectModifiedVars(*stmt, state);
    }

    // ----------------------------------------------------------------
    // Class attribute values
    // ----------------------
    // Every class attribute is declared with an initializer in the class
    // body. A method body referencing an attribute (e.g. `return name`)
    // carries the scopeId and slot of that class scope. When the attribute
    // is never modified, its declared initial value is its constant value.
    // ----------------------------------------------------------------
    collectClassAttributeValues(program, state);

    // -----------------------------------------------------------------------
    // Pass 2
    // ------
    // Walk the program again. Whenever a read of a known constant variable is
    // found, replace that VariableExpr with an equivalent LiteralExpr.
    // -----------------------------------------------------------------------
    for (auto &stmt : program)
    {
        if (!stmt)
            continue;

        replaceConstWithLiteralInStatement(*stmt, state);

        if (auto create = dynamic_cast<CreateVarStmt *>(stmt.get()))
        {
            // --------------------------------------------------------
            // Only variables that are never modified can be propagated.
            // --------------------------------------------------------
            bool isConstant = !state.modifiedVars.contains(create->name);
            if (isConstant && create->value)
            {
                auto cloned = cloneExpr(*create->value);
                if (cloned)
                {
                    state.constVars[create->name] = std::move(cloned);
                }
            }
        }
    }
}

void ConstantPropagator::collectClassAttributeValues(const std::vector<std::unique_ptr<Stmt>> &program, State &state)
{
    for (const auto &stmt : program)
    {
        if (!stmt)
            continue;

        auto createClass = dynamic_cast<const CreateClassStmt *>(stmt.get());
        if (!createClass || !createClass->userClass || !createClass->userClass->class_attributes)
            continue;

        const uint32_t scopeId = createClass->userClass->class_attributes->scopeId;

        for (const auto &member : createClass->body)
        {
            if (!member)
                continue;

            auto attr = dynamic_cast<const CreateVarStmt *>(member.get());
            if (!attr || !attr->value)
                continue;

            // Only attributes whose initializer is a compile-time constant
            // can be propagated into a literal.
            if (!attr->value->canEvaluateAtParseTime())
                continue;

            try
            {
                Value initialValue = attr->value->evaluate(nullptr);
                auto literal = createLiteral(initialValue);
                state.classAttributeValues[scopeId][attr->address.slot] = std::move(literal);
            }
            catch (...)
            {
            }
        }
    }
}

void ConstantPropagator::collectModifiedVars(const Stmt &statement, State &state)
{
    if (auto create = dynamic_cast<const CreateVarStmt *>(&statement))
    {
        if (state.declaredVars.find(create->name) != state.declaredVars.end())
        {
            state.modifiedVars.insert(create->name);
        }
        else
        {
            state.declaredVars.insert(create->name);
        }
        return;
    }

    if (auto update = dynamic_cast<const UpdateVarStmt *>(&statement))
    {
        state.modifiedVars.insert(update->name);
        return;
    }

    if (auto assign = dynamic_cast<const AssignStmt *>(&statement))
    {
        if (assign->target)
            collectModifiedVarsFromExpr(assign->target.get(), state);

        return;
    }

    if (auto updateAsk = dynamic_cast<const UpdateVarWithAskStmt *>(&statement))
    {
        state.modifiedVars.insert(updateAsk->var_name);
        return;
    }

    if (auto createAsk = dynamic_cast<const CreateVarWithAskStmt *>(&statement))
    {
        state.modifiedVars.insert(createAsk->var_name);
        return;
    }

    if (auto ifStmt = dynamic_cast<const IfStmt *>(&statement))
    {
        for (const auto &child : ifStmt->thenBranch)
            if (child)
                collectModifiedVars(*child, state);

        for (const auto &child : ifStmt->otherwiseBranch)
            if (child)
                collectModifiedVars(*child, state);

        return;
    }

    if (auto repeatWhile = dynamic_cast<const RepeatWhileStmt *>(&statement))
    {
        for (const auto &child : repeatWhile->body)
            if (child)
                collectModifiedVars(*child, state);

        return;
    }

    if (auto repeatTimes = dynamic_cast<const RepeatTimesStmt *>(&statement))
    {
        for (const auto &child : repeatTimes->body)
            if (child)
                collectModifiedVars(*child, state);

        return;
    }

    if (auto repeatForever = dynamic_cast<const RepeatForeverStmt *>(&statement))
    {
        for (const auto &child : repeatForever->body)
            if (child)
                collectModifiedVars(*child, state);

        return;
    }

    if (auto forEach = dynamic_cast<const ForEachStmt *>(&statement))
    {
        for (const auto &name : forEach->varNames)
            state.modifiedVars.insert(name);

        for (const auto &child : forEach->body)
            if (child)
                collectModifiedVars(*child, state);

        return;
    }

    if (auto createClass = dynamic_cast<const CreateClassStmt *>(&statement))
    {
        for (const auto &child : createClass->body)
            if (child)
                collectModifiedVars(*child, state);

        return;
    }

    if (auto createMethod = dynamic_cast<const CreateMethodStmt *>(&statement))
    {
        for (const auto &child : createMethod->body)
            if (child)
                collectModifiedVars(*child, state);

        return;
    }

    if (auto tryStmt = dynamic_cast<const TryStmt *>(&statement))
    {
        for (const auto &child : tryStmt->tryBody)
            if (child)
                collectModifiedVars(*child, state);

        for (const auto &[type, body] : tryStmt->catchClauses)
            for (const auto &child : body)
                if (child)
                    collectModifiedVars(*child, state);

        for (const auto &child : tryStmt->alwaysBody)
            if (child)
                collectModifiedVars(*child, state);

        return;
    }
}

void ConstantPropagator::collectModifiedVarsFromExpr(const Expr *expression, State &state)
{
    if (!expression)
        return;

    if (auto varExpr = dynamic_cast<const VariableExpr *>(expression))
    {
        state.modifiedVars.insert(varExpr->name);
        return;
    }

    if (auto getAttr = dynamic_cast<const GetAttributeExpr *>(expression))
    {
        collectModifiedVarsFromExpr(getAttr->object.get(), state);

        // An attribute write (user.name = 'you') makes that attribute
        // non-constant everywhere, so it must never be propagated.
        state.modifiedVars.insert(getAttr->attribute);
        return;
    }

    if (auto indexExpr = dynamic_cast<const IndexExpr *>(expression))
    {
        collectModifiedVarsFromExpr(indexExpr->collection.get(), state);
        return;
    }
}

void ConstantPropagator::replaceConstWithLiteralInStatement(Stmt &statement, State &state)
{
    if (auto create = dynamic_cast<CreateVarStmt *>(&statement))
    {
        create->value = replaceConstWithLiteralInExpression(std::move(create->value), state);
        return;
    }

    if (auto update = dynamic_cast<UpdateVarStmt *>(&statement))
    {
        update->value = replaceConstWithLiteralInExpression(std::move(update->value), state);
        return;
    }

    if (auto assign = dynamic_cast<AssignStmt *>(&statement))
    {
        assign->value = replaceConstWithLiteralInExpression(std::move(assign->value), state);
        return;
    }

    if (auto printStmt = dynamic_cast<PrintStmt *>(&statement))
    {
        printStmt->message = replaceConstWithLiteralInExpression(std::move(printStmt->message), state);
        return;
    }

    if (auto returnStmt = dynamic_cast<ReturnStmt *>(&statement))
    {
        returnStmt->value = replaceConstWithLiteralInExpression(std::move(returnStmt->value), state);
        return;
    }

    if (auto ifStmt = dynamic_cast<IfStmt *>(&statement))
    {
        ifStmt->condition = replaceConstWithLiteralInExpression(std::move(ifStmt->condition), state);
        for (auto &child : ifStmt->thenBranch)
            if (child)
                replaceConstWithLiteralInStatement(*child, state);

        for (auto &child : ifStmt->otherwiseBranch)
            if (child)
                replaceConstWithLiteralInStatement(*child, state);

        return;
    }

    if (auto repeatWhile = dynamic_cast<RepeatWhileStmt *>(&statement))
    {
        repeatWhile->condition = replaceConstWithLiteralInExpression(std::move(repeatWhile->condition), state);
        for (auto &child : repeatWhile->body)
            if (child)
                replaceConstWithLiteralInStatement(*child, state);

        return;
    }

    if (auto repeatTimes = dynamic_cast<RepeatTimesStmt *>(&statement))
    {
        repeatTimes->countExpr = replaceConstWithLiteralInExpression(std::move(repeatTimes->countExpr), state);
        for (auto &child : repeatTimes->body)
            if (child)
                replaceConstWithLiteralInStatement(*child, state);

        return;
    }

    if (auto repeatForever = dynamic_cast<RepeatForeverStmt *>(&statement))
    {
        for (auto &child : repeatForever->body)
            if (child)
                replaceConstWithLiteralInStatement(*child, state);

        return;
    }

    if (auto forEach = dynamic_cast<ForEachStmt *>(&statement))
    {
        forEach->iterable = replaceConstWithLiteralInExpression(std::move(forEach->iterable), state);
        for (auto &child : forEach->body)
            if (child)
                replaceConstWithLiteralInStatement(*child, state);

        return;
    }

    if (auto createClass = dynamic_cast<CreateClassStmt *>(&statement))
    {
        for (auto &child : createClass->body)
            if (child)
                replaceConstWithLiteralInStatement(*child, state);

        return;
    }

    if (auto createMethod = dynamic_cast<CreateMethodStmt *>(&statement))
    {
        for (auto &child : createMethod->body)
            if (child)
                replaceConstWithLiteralInStatement(*child, state);

        return;
    }

    if (auto tryStmt = dynamic_cast<TryStmt *>(&statement))
    {
        for (auto &child : tryStmt->tryBody)
            if (child)
                replaceConstWithLiteralInStatement(*child, state);

        for (auto &[type, body] : tryStmt->catchClauses)
            for (auto &child : body)
                if (child)
                    replaceConstWithLiteralInStatement(*child, state);

        for (auto &child : tryStmt->alwaysBody)
            if (child)
                replaceConstWithLiteralInStatement(*child, state);

        return;
    }

    if (auto resist = dynamic_cast<ResistStmt *>(&statement))
    {
        resist->messageExpr = replaceConstWithLiteralInExpression(std::move(resist->messageExpr), state);
        return;
    }
}

std::unique_ptr<Expr> ConstantPropagator::replaceConstWithLiteralInExpression(
    std::unique_ptr<Expr> expression, State &state)
{
    if (!expression)
        return nullptr;

    // ---------------------------------------------------------------
    // This is the heart of Constant Propagator
    //
    // If it is a VariableExpr (read of a variable),
    // let's check if it has a constant value (literalExpr).
    // If so, let's replace it with its contant value directly.
    //
    // In other words:
    //
    //     If this expression is a variable,
    //         and I know its constant value,
    //             replace the variable with a copy of that constant.
    //     Otherwise,
    //         leave the expression unchanged.
    // ---------------------------------------------------------------
    if (auto varExpr = dynamic_cast<VariableExpr *>(expression.get()))
    {
        auto it = state.constVars.find(varExpr->name);
        if (it != state.constVars.end() && it->second)
        {
            return cloneExpr(*it->second);
        }

        // ----------------------------------------------------------
        // Class attributes are constant when they are never modified.
        //
        // A reference to an attribute ('return name' inside a method)
        // carries the scopeId and slot of the class attributes scope,
        // under which its declared initial value is recorded.
        // ----------------------------------------------------------
        if (varExpr->name != SELF_VARIABLE)
        {
            auto scopeIt = state.classAttributeValues.find(varExpr->address.scopeId);
            if (scopeIt != state.classAttributeValues.end())
            {
                auto slotIt = scopeIt->second.find(varExpr->address.slot);
                if (slotIt != scopeIt->second.end() && !state.modifiedVars.contains(varExpr->name))
                {
                    return cloneExpr(*slotIt->second);
                }
            }
        }

        return expression;
    }

    if (auto grouping = dynamic_cast<GroupingExpr *>(expression.get()))
    {
        grouping->expression = replaceConstWithLiteralInExpression(std::move(grouping->expression), state);
        return expression;
    }

    if (auto unary = dynamic_cast<UnaryExpr *>(expression.get()))
    {
        unary->right = replaceConstWithLiteralInExpression(std::move(unary->right), state);
        return expression;
    }

    if (auto binary = dynamic_cast<BinaryExpr *>(expression.get()))
    {
        binary->left = replaceConstWithLiteralInExpression(std::move(binary->left), state);
        binary->right = replaceConstWithLiteralInExpression(std::move(binary->right), state);

        return expression;
    }

    if (auto listExpr = dynamic_cast<ListExpr *>(expression.get()))
    {
        for (auto &el : listExpr->elements)
            el = replaceConstWithLiteralInExpression(std::move(el), state);

        return expression;
    }

    if (auto dictExpr = dynamic_cast<DictExpr *>(expression.get()))
    {
        for (auto &[k, v] : dictExpr->entries)
        {
            k = replaceConstWithLiteralInExpression(std::move(k), state);
            v = replaceConstWithLiteralInExpression(std::move(v), state);
        }

        return expression;
    }

    if (auto condExpr = dynamic_cast<ConditionalExpr *>(expression.get()))
    {
        condExpr->condition = replaceConstWithLiteralInExpression(std::move(condExpr->condition), state);
        condExpr->thenBranch = replaceConstWithLiteralInExpression(std::move(condExpr->thenBranch), state);
        condExpr->elseBranch = replaceConstWithLiteralInExpression(std::move(condExpr->elseBranch), state);

        return expression;
    }

    if (auto methodCall = dynamic_cast<MethodCallExpr *>(expression.get()))
    {
        if (methodCall->object)
            methodCall->object = replaceConstWithLiteralInExpression(std::move(methodCall->object), state);

        for (auto &arg : methodCall->args)
            arg = replaceConstWithLiteralInExpression(std::move(arg), state);

        return expression;
    }

    if (auto getAttr = dynamic_cast<GetAttributeExpr *>(expression.get()))
    {
        if (getAttr->object)
            getAttr->object = replaceConstWithLiteralInExpression(std::move(getAttr->object), state);

        return expression;
    }

    if (auto indexExpr = dynamic_cast<IndexExpr *>(expression.get()))
    {
        if (indexExpr->collection)
            indexExpr->collection = replaceConstWithLiteralInExpression(std::move(indexExpr->collection), state);

        if (indexExpr->index)
            indexExpr->index = replaceConstWithLiteralInExpression(std::move(indexExpr->index), state);

        return expression;
    }

    if (auto fmtString = dynamic_cast<FormattedStringExpr *>(expression.get()))
    {
        for (auto &expr : fmtString->expressions)
            expr = replaceConstWithLiteralInExpression(std::move(expr), state);

        return expression;
    }

    return expression;
}

std::unique_ptr<Expr> ConstantPropagator::cloneExpr(const Expr &expr)
{
    if (auto lit = dynamic_cast<const LiteralExpr *>(&expr))
    {
        return std::make_unique<LiteralExpr>(lit->value, lit->type);
    }

    if (expr.canEvaluateAtParseTime())
    {
        try
        {
            Value val = expr.evaluate(nullptr);
            return createLiteral(val);
        }
        catch (...)
        {
        }
    }

    return nullptr;
}

std::unique_ptr<Expr> ConstantPropagator::createLiteral(const Value &value)
{
    std::shared_ptr<CreationType> type = KnownTypes::NOTHING;
    if (value.IS_INT)
        type = KnownTypes::INT;

    else if (value.IS_DOUBLE)
        type = KnownTypes::DOUBLE;

    else if (value.IS_BOOLEAN)
        type = KnownTypes::BOOLEAN;

    else if (value.IS_STRING)
        type = KnownTypes::STRING;

    return std::make_unique<LiteralExpr>(value, type);
}
