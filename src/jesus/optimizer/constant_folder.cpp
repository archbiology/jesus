#include "constant_folder.hpp"

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

#include "ast/expr/literal_expr.hpp"
#include "ast/expr/grouping_expr.hpp"
#include "ast/expr/unary_expr.hpp"
#include "ast/expr/binary_expr.hpp"
#include "ast/expr/list_expr.hpp"
#include "ast/expr/dict_expr.hpp"
#include "ast/expr/conditional_expr.hpp"
// #include "ast/expr/formatted_string_expr.hpp"
#include "ast/expr/method_call_expr.hpp"
#include "ast/expr/get_attr_expr.hpp"
#include "ast/expr/index_expr.hpp"

#include "types/known_types.hpp"

void ConstantFolder::run(std::vector<std::unique_ptr<Stmt>> &program)
{
    for (auto &stmt : program)
    {
        if (stmt)
            optimizeStatement(*stmt);
    }
}

void ConstantFolder::optimizeStatement(Stmt &statement)
{
    if (auto create = dynamic_cast<CreateVarStmt *>(&statement))
    {
        create->value = optimizeExpression(std::move(create->value));
        return;
    }

    if (auto update = dynamic_cast<UpdateVarStmt *>(&statement))
    {
        update->value = optimizeExpression(std::move(update->value));
        return;
    }

    if (auto assign = dynamic_cast<AssignStmt *>(&statement))
    {
        assign->value = optimizeExpression(std::move(assign->value));
        return;
    }

    if (auto printStmt = dynamic_cast<PrintStmt *>(&statement))
    {
        printStmt->message = optimizeExpression(std::move(printStmt->message));
        return;
    }

    if (auto returnStmt = dynamic_cast<ReturnStmt *>(&statement))
    {
        returnStmt->value = optimizeExpression(std::move(returnStmt->value));
        return;
    }

    if (auto ifStmt = dynamic_cast<IfStmt *>(&statement))
    {
        ifStmt->condition = optimizeExpression(std::move(ifStmt->condition));

        for (auto &child : ifStmt->thenBranch)
            if (child)
                optimizeStatement(*child);

        for (auto &child : ifStmt->otherwiseBranch)
            if (child)
                optimizeStatement(*child);

        return;
    }

    if (auto repeatWhile = dynamic_cast<RepeatWhileStmt *>(&statement))
    {
        repeatWhile->condition = optimizeExpression(std::move(repeatWhile->condition));
        for (auto &child : repeatWhile->body)
            if (child)
                optimizeStatement(*child);
        return;
    }

    if (auto repeatTimes = dynamic_cast<RepeatTimesStmt *>(&statement))
    {
        repeatTimes->countExpr = optimizeExpression(std::move(repeatTimes->countExpr));
        for (auto &child : repeatTimes->body)
            if (child)
                optimizeStatement(*child);
        return;
    }

    if (auto repeatForever = dynamic_cast<RepeatForeverStmt *>(&statement))
    {
        for (auto &child : repeatForever->body)
            if (child)
                optimizeStatement(*child);
        return;
    }

    if (auto forEach = dynamic_cast<ForEachStmt *>(&statement))
    {
        forEach->iterable = optimizeExpression(std::move(forEach->iterable));
        for (auto &child : forEach->body)
            if (child)
                optimizeStatement(*child);
        return;
    }

    if (auto createClass = dynamic_cast<CreateClassStmt *>(&statement))
    {
        for (auto &child : createClass->body)
            if (child)
                optimizeStatement(*child);
        return;
    }

    if (auto createMethod = dynamic_cast<CreateMethodStmt *>(&statement))
    {
        for (auto &child : createMethod->body)
            if (child)
                optimizeStatement(*child);
        return;
    }

    if (auto tryStmt = dynamic_cast<TryStmt *>(&statement))
    {
        for (auto &child : tryStmt->tryBody)
            if (child)
                optimizeStatement(*child);

        for (auto &[type, body] : tryStmt->catchClauses)
            for (auto &child : body)
                if (child)
                    optimizeStatement(*child);

        for (auto &child : tryStmt->alwaysBody)
            if (child)
                optimizeStatement(*child);

        return;
    }

    if (auto resist = dynamic_cast<ResistStmt *>(&statement))
    {
        resist->messageExpr = optimizeExpression(std::move(resist->messageExpr));
        return;
    }
}

std::unique_ptr<Expr> ConstantFolder::optimizeExpression(std::unique_ptr<Expr> expression)
{
    if (!expression)
        return nullptr;

    if (auto grouping = dynamic_cast<GroupingExpr *>(expression.get()))
    {
        grouping->expression = optimizeExpression(std::move(grouping->expression));
        if (grouping->canEvaluateAtParseTime())
        {
            try
            {
                auto value = grouping->evaluate(nullptr);
                return createLiteral(value);
            }
            catch (...)
            {
            }
        }
        return expression;
    }

    if (auto unary = dynamic_cast<UnaryExpr *>(expression.get()))
    {
        unary->right = optimizeExpression(std::move(unary->right));
        if (unary->canEvaluateAtParseTime())
        {
            try
            {
                auto value = unary->evaluate(nullptr);
                return createLiteral(value);
            }
            catch (...)
            {
            }
        }
        return expression;
    }

    if (auto binary = dynamic_cast<BinaryExpr *>(expression.get()))
    {
        return optimizeBinaryExpr(
            std::unique_ptr<BinaryExpr>(static_cast<BinaryExpr *>(expression.release())));
    }

    if (auto listExpr = dynamic_cast<ListExpr *>(expression.get()))
    {
        for (auto &el : listExpr->elements)
            el = optimizeExpression(std::move(el));

        return expression;
    }

    if (auto dictExpr = dynamic_cast<DictExpr *>(expression.get()))
    {
        for (auto &[k, v] : dictExpr->entries)
        {
            k = optimizeExpression(std::move(k));
            v = optimizeExpression(std::move(v));
        }
        return expression;
    }

    if (auto condExpr = dynamic_cast<ConditionalExpr *>(expression.get()))
    {
        condExpr->condition = optimizeExpression(std::move(condExpr->condition));
        condExpr->thenBranch = optimizeExpression(std::move(condExpr->thenBranch));
        condExpr->elseBranch = optimizeExpression(std::move(condExpr->elseBranch));

        if (condExpr->canEvaluateAtParseTime())
        {
            try
            {
                auto value = condExpr->evaluate(nullptr);
                return createLiteral(value);
            }
            catch (...)
            {
            }
        }
        return expression;
    }

    if (auto methodCall = dynamic_cast<MethodCallExpr *>(expression.get()))
    {
        if (methodCall->object)
            methodCall->object = optimizeExpression(std::move(methodCall->object));

        for (auto &arg : methodCall->args)
            arg = optimizeExpression(std::move(arg));

        return expression;
    }

    if (auto getAttr = dynamic_cast<GetAttributeExpr *>(expression.get()))
    {
        if (getAttr->object)
            getAttr->object = optimizeExpression(std::move(getAttr->object));

        return expression;
    }

    if (auto indexExpr = dynamic_cast<IndexExpr *>(expression.get()))
    {
        if (indexExpr->collection)
            indexExpr->collection = optimizeExpression(std::move(indexExpr->collection));

        if (indexExpr->index)
            indexExpr->index = optimizeExpression(std::move(indexExpr->index));

        return expression;
    }

    return expression;
}

std::unique_ptr<Expr> ConstantFolder::optimizeBinaryExpr(std::unique_ptr<BinaryExpr> expression)
{
    expression->left = optimizeExpression(std::move(expression->left));
    expression->right = optimizeExpression(std::move(expression->right));

    if (!expression->canEvaluateAtParseTime())
        return expression;

    try
    {
        auto value = expression->evaluate(nullptr);
        return createLiteral(value);
    }
    catch (...)
    {
    }

    return expression;
}

std::unique_ptr<Expr> ConstantFolder::createLiteral(const Value &value)
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
