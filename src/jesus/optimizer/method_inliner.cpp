#include "method_inliner.hpp"
#include "interpreter/runtime/method.hpp"

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
#include "ast/expr/ask_expr.hpp"
#include "ast/expr/create_instance_expr.hpp"
#include "ast/expr/parity_check_expr.hpp"
#include "ast/expr/convert_to_expr.hpp"

void MethodInliner::run(std::vector<std::unique_ptr<Stmt>> &program)
{
    std::unordered_map<std::string, InlinableMethod> knownMethods;

    // Pass 1: Collect method definitions
    for (const auto &stmt : program)
    {
        if (stmt)
            collectMethodsFromStmt(*stmt, knownMethods);
    }

    // Pass 2: Inline method calls across statements and expressions
    optimizeBlock(program, knownMethods);
}

void MethodInliner::collectMethodsFromStmt(
    const Stmt &statement, std::unordered_map<std::string, InlinableMethod> &knownMethods)
{
    // ----------------------------------------------------
    // Methods in Jesus are always declared inside classes.
    // Therefore only class bodies need to be scanned.
    // ----------------------------------------------------
    if (auto createClass = dynamic_cast<const CreateClassStmt *>(&statement))
    {
        const Heart *classAttributes = nullptr;
        if (createClass->userClass)
            classAttributes = createClass->userClass->class_attributes.get();

        for (const auto &stmt : createClass->body)
        {
            if (stmt)
            {
                if (auto method = dynamic_cast<const CreateMethodStmt *>(stmt.get()))
                {
                    if (method->isConstructor)
                        continue;

                    // FIXME: two classes may have the same method names
                    knownMethods[method->name] = {method, classAttributes};
                }
            }
        }
        return;
    }
}

void MethodInliner::optimizeBlock(
    std::vector<std::unique_ptr<Stmt>> &stmts, const std::unordered_map<std::string, InlinableMethod> &knownMethods)
{
    for (auto &stmt : stmts)
    {
        if (stmt)
            optimizeStatement(*stmt, knownMethods);
    }
}

void MethodInliner::optimizeBlock(
    std::vector<std::shared_ptr<Stmt>> &stmts, const std::unordered_map<std::string, InlinableMethod> &knownMethods)
{
    for (auto &stmt : stmts)
    {
        if (stmt)
            optimizeStatement(*stmt, knownMethods);
    }
}

void MethodInliner::optimizeStatement(
    Stmt &statement, const std::unordered_map<std::string, InlinableMethod> &knownMethods)
{
    if (auto create = dynamic_cast<CreateVarStmt *>(&statement))
    {
        create->value = optimizeExpression(std::move(create->value), knownMethods);
        return;
    }

    if (auto update = dynamic_cast<UpdateVarStmt *>(&statement))
    {
        update->value = optimizeExpression(std::move(update->value), knownMethods);
        return;
    }

    if (auto assign = dynamic_cast<AssignStmt *>(&statement))
    {
        if (assign->target)
        {
            auto optTarget = optimizeExpression(std::move(assign->target), knownMethods);
            if (auto assignable = dynamic_cast<AssignableExpr *>(optTarget.get()))
            {
                optTarget.release();
                assign->target.reset(assignable);
            }
        }
        assign->value = optimizeExpression(std::move(assign->value), knownMethods);
        return;
    }

    if (auto printStmt = dynamic_cast<PrintStmt *>(&statement))
    {
        printStmt->message = optimizeExpression(std::move(printStmt->message), knownMethods);
        return;
    }

    if (auto returnStmt = dynamic_cast<ReturnStmt *>(&statement))
    {
        returnStmt->value = optimizeExpression(std::move(returnStmt->value), knownMethods);
        return;
    }

    if (auto resist = dynamic_cast<ResistStmt *>(&statement))
    {
        resist->messageExpr = optimizeExpression(std::move(resist->messageExpr), knownMethods);
        return;
    }

    if (auto createAsk = dynamic_cast<CreateVarWithAskStmt *>(&statement))
    {
        return;
    }

    if (auto updateAsk = dynamic_cast<UpdateVarWithAskStmt *>(&statement))
    {
        return;
    }

    if (auto ifStmt = dynamic_cast<IfStmt *>(&statement))
    {
        ifStmt->condition = optimizeExpression(std::move(ifStmt->condition), knownMethods);
        optimizeBlock(ifStmt->thenBranch, knownMethods);
        optimizeBlock(ifStmt->otherwiseBranch, knownMethods);
        return;
    }

    if (auto repeatWhile = dynamic_cast<RepeatWhileStmt *>(&statement))
    {
        repeatWhile->condition = optimizeExpression(std::move(repeatWhile->condition), knownMethods);
        optimizeBlock(repeatWhile->body, knownMethods);
        return;
    }

    if (auto repeatTimes = dynamic_cast<RepeatTimesStmt *>(&statement))
    {
        repeatTimes->countExpr = optimizeExpression(std::move(repeatTimes->countExpr), knownMethods);
        optimizeBlock(repeatTimes->body, knownMethods);
        return;
    }

    if (auto repeatForever = dynamic_cast<RepeatForeverStmt *>(&statement))
    {
        optimizeBlock(repeatForever->body, knownMethods);
        return;
    }

    if (auto forEach = dynamic_cast<ForEachStmt *>(&statement))
    {
        forEach->iterable = optimizeExpression(std::move(forEach->iterable), knownMethods);
        optimizeBlock(forEach->body, knownMethods);
        return;
    }

    if (auto createClass = dynamic_cast<CreateClassStmt *>(&statement))
    {
        optimizeBlock(createClass->body, knownMethods);
        return;
    }

    if (auto createMethod = dynamic_cast<CreateMethodStmt *>(&statement))
    {
        optimizeBlock(createMethod->body, knownMethods);
        return;
    }

    if (auto tryStmt = dynamic_cast<TryStmt *>(&statement))
    {
        optimizeBlock(tryStmt->tryBody, knownMethods);
        for (auto &[type, catchBody] : tryStmt->catchClauses)
        {
            optimizeBlock(catchBody, knownMethods);
        }
        optimizeBlock(tryStmt->alwaysBody, knownMethods);
        return;
    }
}

std::unique_ptr<Expr> MethodInliner::optimizeExpression(
    std::unique_ptr<Expr> expression, const std::unordered_map<std::string, InlinableMethod> &knownMethods)
{
    if (!expression)
        return nullptr;

    if (auto methodCall = dynamic_cast<MethodCallExpr *>(expression.get()))
    {
        std::unique_ptr<MethodCallExpr> call(static_cast<MethodCallExpr *>(expression.release()));
        return inlineMethodCall(std::move(call), knownMethods);
    }

    if (auto grouping = dynamic_cast<GroupingExpr *>(expression.get()))
    {
        grouping->expression = optimizeExpression(std::move(grouping->expression), knownMethods);
        return expression;
    }

    if (auto unary = dynamic_cast<UnaryExpr *>(expression.get()))
    {
        unary->right = optimizeExpression(std::move(unary->right), knownMethods);
        return expression;
    }

    if (auto binary = dynamic_cast<BinaryExpr *>(expression.get()))
    {
        binary->left = optimizeExpression(std::move(binary->left), knownMethods);
        binary->right = optimizeExpression(std::move(binary->right), knownMethods);
        return expression;
    }

    if (auto listExpr = dynamic_cast<ListExpr *>(expression.get()))
    {
        for (auto &el : listExpr->elements)
            el = optimizeExpression(std::move(el), knownMethods);

        return expression;
    }

    if (auto dictExpr = dynamic_cast<DictExpr *>(expression.get()))
    {
        for (auto &[key, value] : dictExpr->entries)
        {
            key = optimizeExpression(std::move(key), knownMethods);
            value = optimizeExpression(std::move(value), knownMethods);
        }
        return expression;
    }

    if (auto condExpr = dynamic_cast<ConditionalExpr *>(expression.get()))
    {
        condExpr->condition = optimizeExpression(std::move(condExpr->condition), knownMethods);
        condExpr->thenBranch = optimizeExpression(std::move(condExpr->thenBranch), knownMethods);
        condExpr->elseBranch = optimizeExpression(std::move(condExpr->elseBranch), knownMethods);
        return expression;
    }

    if (auto getAttr = dynamic_cast<GetAttributeExpr *>(expression.get()))
    {
        if (getAttr->object)
            getAttr->object = optimizeExpression(std::move(getAttr->object), knownMethods);

        return expression;
    }

    if (auto indexExpr = dynamic_cast<IndexExpr *>(expression.get()))
    {
        if (indexExpr->collection)
            indexExpr->collection = optimizeExpression(std::move(indexExpr->collection), knownMethods);

        if (indexExpr->index)
            indexExpr->index = optimizeExpression(std::move(indexExpr->index), knownMethods);

        return expression;
    }

    if (auto fmtString = dynamic_cast<FormattedStringExpr *>(expression.get()))
    {
        for (auto &expr : fmtString->expressions)
            expr = optimizeExpression(std::move(expr), knownMethods);

        return expression;
    }

    if (auto parity = dynamic_cast<ParityCheckExpr *>(expression.get()))
    {
        if (parity->target)
            parity->target = optimizeExpression(std::move(parity->target), knownMethods);

        return expression;
    }

    if (auto ask = dynamic_cast<AskExpr *>(expression.get()))
    {
        if (ask->prompt)
            ask->prompt = optimizeExpression(std::move(ask->prompt), knownMethods);

        return expression;
    }

    if (auto createInst = dynamic_cast<CreateInstanceExpr *>(expression.get()))
    {
        if (createInst->constructorArgs)
            createInst->constructorArgs = optimizeExpression(std::move(createInst->constructorArgs), knownMethods);

        return expression;
    }

    if (auto convert = dynamic_cast<ConvertToExpr *>(expression.get()))
    {
        if (convert->valueExpr)
            convert->valueExpr = optimizeExpression(std::move(convert->valueExpr), knownMethods);

        return expression;
    }

    return expression;
}

std::unique_ptr<Expr> MethodInliner::inlineMethodCall(
    std::unique_ptr<MethodCallExpr> methodCall, const std::unordered_map<std::string, InlinableMethod> &knownMethods)
{
    if (methodCall->object)
        methodCall->object = optimizeExpression(std::move(methodCall->object), knownMethods);

    for (auto &arg : methodCall->args)
        arg = optimizeExpression(std::move(arg), knownMethods);

    const std::vector<std::shared_ptr<Stmt>> *methodBody = nullptr;
    const Heart *methodParams = nullptr;
    std::string methodName;

    if (methodCall->method)
        methodName = methodCall->method->name;

    // TODO: Inline also NativeMethod
    if (auto method = dynamic_cast<Method *>(methodCall->method.get()))
    {
        methodBody = &method->body;
        methodParams = method->params.get();
    }

    // ---------------------------------------------------------
    // Find the class attributes of the method being called, so
    // that references to instance attributes can be rewritten as
    // attribute accesses on the receiver object (user.name).
    // ---------------------------------------------------------
    const Heart *classAttributes = nullptr;
    auto knownMethod = knownMethods.find(methodName);
    if (knownMethod != knownMethods.end())
        classAttributes = knownMethod->second.classAttributes;

    if (methodBody && methodBody->size() == 1)
    {
        // -----------------------------------------------
        // Currently only methods consisting of a single
        // `return expression` are inlined.
        //
        // Methods whose body performs actions (say, warn,
        // update variables, loops, etc.) are intentionally
        // left untouched.
        //
        // If MethodCallStmt has to be replaced with SayStmt,
        // that is modifying statements, not expressions.
        // We only optimize expressions for now.
        // -----------------------------------------------
        if (auto retStmt = dynamic_cast<const ReturnStmt *>((*methodBody)[0].get()))
        {
            if (retStmt->value)
            {
                /**
                 * @brief argumentValues["x"] = LiteralExpr(2);
                 */
                std::unordered_map<std::string, const Expr *> argumentValues;
                if (methodParams)
                {
                    const auto &paramNames = methodParams->getVariableNames();
                    for (size_t i = 0; i < paramNames.size() && i < methodCall->args.size(); ++i)
                    {
                        argumentValues[paramNames[i]] = methodCall->args[i].get();
                    }
                }

                auto inlined = cloneExpressionReplacingParamsWithArgs(
                    *retStmt->value, argumentValues, methodCall->object.get(), classAttributes);
                if (inlined)
                {
                    return optimizeExpression(std::move(inlined), knownMethods);
                }
            }
        }
    }

    return methodCall;
}

std::unique_ptr<Expr> MethodInliner::cloneExpressionReplacingParamsWithArgs(
    const Expr &expression,
    const std::unordered_map<std::string, const Expr *> &argumentValues,
    const Expr *objectExpr,
    const Heart *classAttributes)
{
    //--------------------------------------------------------------
    // Parameter substitution.
    //
    // VariableExpr is the only AST node whose meaning may change.
    // If the variable is a method parameter, replace it with the
    // corresponding argument expression.
    //--------------------------------------------------------------
    if (auto varExpr = dynamic_cast<const VariableExpr *>(&expression))
    {
        // -------------------------------------
        // This is the 'heart' of this method,
        // replacing params with their values
        //
        // it = std::pair<Key, Value>
        // it = std::pair<first, second>
        // it->first --> param name
        // it->second --> arg value
        // argumentValues["x"] = LiteralExpr(2);
        // -------------------------------------
        auto it = argumentValues.find(varExpr->name);
        if (it != argumentValues.end() && it->second)
        {
            // This call clones the literal argument value (it->second is the value)
            return cloneExpressionReplacingParamsWithArgs(*it->second, {}, nullptr, classAttributes);
        }
        // The 'heart' of the method ends here.
        // ------------------------------------

        if ((varExpr->name == "my" || varExpr->name == "this" || varExpr->name == "I" || varExpr->name == "self") &&
            objectExpr)
        {
            return cloneExpressionReplacingParamsWithArgs(*objectExpr, {}, nullptr, classAttributes);
        }

        // ----------------------------------------------------------
        // An instance attribute is rewritten as an attribute access on
        // the receiver object: 'name' becomes 'user.name'.
        //
        // The parse-time class scope is not reachable from the call
        // site, so the reference must be resolved through the instance.
        // ----------------------------------------------------------
        if (classAttributes && classAttributes->varExistsInHierarchy(varExpr->name))
        {
            if (!objectExpr)
                return nullptr;

            return std::make_unique<GetAttributeExpr>(
                cloneExpressionReplacingParamsWithArgs(*objectExpr, {}, nullptr, classAttributes),
                varExpr->name,
                varExpr->address);
        }

        // A global variable keeps its reference: its address is still
        // valid at the call site.
        return std::make_unique<VariableExpr>(varExpr->address, varExpr->name);
    }

    // ----------------------------------------------------------
    // Literal cloning
    //----------------
    // Why clone the literal instead of reusing it?
    //
    // The original LiteralExpr belongs to the method definition.
    // We don't modify the body of existing methods
    // The inlined expression needs its own copy.
    // ----------------------------------------------------------
    if (auto lit = dynamic_cast<const LiteralExpr *>(&expression))
    {
        return std::make_unique<LiteralExpr>(lit->value, lit->type);
    }

    //---------------------------------------------------------
    // Recursive cloning compound expressions.
    //
    // All remaining expression types preserve their semantics.
    // We simply clone the current node and recursively clone
    // its child expressions.
    //---------------------------------------------------------
    if (auto grouping = dynamic_cast<const GroupingExpr *>(&expression))
    {
        return std::make_unique<GroupingExpr>(
            cloneExpressionReplacingParamsWithArgs(*grouping->expression, argumentValues, objectExpr, classAttributes));
    }

    if (auto unary = dynamic_cast<const UnaryExpr *>(&expression))
    {
        return std::make_unique<UnaryExpr>(
            unary->op,
            cloneExpressionReplacingParamsWithArgs(*unary->right, argumentValues, objectExpr, classAttributes));
    }

    if (auto binary = dynamic_cast<const BinaryExpr *>(&expression))
    {
        return std::make_unique<BinaryExpr>(
            cloneExpressionReplacingParamsWithArgs(*binary->left, argumentValues, objectExpr, classAttributes),
            binary->op,
            cloneExpressionReplacingParamsWithArgs(*binary->right, argumentValues, objectExpr, classAttributes));
    }

    if (auto listExpr = dynamic_cast<const ListExpr *>(&expression))
    {
        std::vector<std::unique_ptr<Expr>> elements;
        for (const auto &el : listExpr->elements)
            if (el)
                elements.push_back(
                    cloneExpressionReplacingParamsWithArgs(*el, argumentValues, objectExpr, classAttributes));

        return std::make_unique<ListExpr>(std::move(elements), listExpr->listType);
    }

    if (auto dictExpr = dynamic_cast<const DictExpr *>(&expression))
    {
        std::vector<std::pair<std::unique_ptr<Expr>, std::unique_ptr<Expr>>> entries;
        for (const auto &[key, value] : dictExpr->entries)
        {
            auto newK = key ? cloneExpressionReplacingParamsWithArgs(*key, argumentValues, objectExpr, classAttributes)
                            : nullptr;
            auto newV =
                value ? cloneExpressionReplacingParamsWithArgs(*value, argumentValues, objectExpr, classAttributes)
                      : nullptr;
            entries.push_back({std::move(newK), std::move(newV)});
        }

        return std::make_unique<DictExpr>(std::move(entries), dictExpr->dictType);
    }

    if (auto condExpr = dynamic_cast<const ConditionalExpr *>(&expression))
    {
        return std::make_unique<ConditionalExpr>(
            cloneExpressionReplacingParamsWithArgs(*condExpr->condition, argumentValues, objectExpr, classAttributes),
            cloneExpressionReplacingParamsWithArgs(*condExpr->thenBranch, argumentValues, objectExpr, classAttributes),
            cloneExpressionReplacingParamsWithArgs(*condExpr->elseBranch, argumentValues, objectExpr, classAttributes));
    }

    if (auto getAttr = dynamic_cast<const GetAttributeExpr *>(&expression))
    {
        std::unique_ptr<Expr> newObj =
            getAttr->object
                ? cloneExpressionReplacingParamsWithArgs(*getAttr->object, argumentValues, objectExpr, classAttributes)
                : nullptr;

        return std::make_unique<GetAttributeExpr>(std::move(newObj), getAttr->attribute, getAttr->address);
    }

    if (auto indexExpr = dynamic_cast<const IndexExpr *>(&expression))
    {
        std::unique_ptr<Expr> newColl = indexExpr->collection
                                            ? cloneExpressionReplacingParamsWithArgs(
                                                  *indexExpr->collection, argumentValues, objectExpr, classAttributes)
                                            : nullptr;

        std::unique_ptr<Expr> newIdx =
            indexExpr->index
                ? cloneExpressionReplacingParamsWithArgs(*indexExpr->index, argumentValues, objectExpr, classAttributes)
                : nullptr;

        return std::make_unique<IndexExpr>(std::move(newColl), std::move(newIdx));
    }

    if (auto fmtString = dynamic_cast<const FormattedStringExpr *>(&expression))
    {
        std::vector<std::unique_ptr<Expr>> exprs;
        for (const auto &e : fmtString->expressions)
            if (e)
                exprs.push_back(
                    cloneExpressionReplacingParamsWithArgs(*e, argumentValues, objectExpr, classAttributes));

        return std::make_unique<FormattedStringExpr>(fmtString->raw, fmtString->parts, std::move(exprs));
    }

    if (auto parity = dynamic_cast<const ParityCheckExpr *>(&expression))
    {
        std::unique_ptr<Expr> newTarget =
            parity->target
                ? cloneExpressionReplacingParamsWithArgs(*parity->target, argumentValues, objectExpr, classAttributes)
                : nullptr;

        return std::make_unique<ParityCheckExpr>(std::move(newTarget), parity->negate, parity->checkOdd);
    }

    if (auto ask = dynamic_cast<const AskExpr *>(&expression))
    {
        std::unique_ptr<Expr> newPrompt =
            ask->prompt
                ? cloneExpressionReplacingParamsWithArgs(*ask->prompt, argumentValues, objectExpr, classAttributes)
                : nullptr;

        return std::make_unique<AskExpr>(std::move(newPrompt));
    }

    if (auto createInst = dynamic_cast<const CreateInstanceExpr *>(&expression))
    {
        std::unique_ptr<Expr> newArgs =
            createInst->constructorArgs ? cloneExpressionReplacingParamsWithArgs(
                                              *createInst->constructorArgs, argumentValues, objectExpr, classAttributes)
                                        : nullptr;

        return std::make_unique<CreateInstanceExpr>(createInst->name, createInst->klass, std::move(newArgs));
    }

    if (auto convert = dynamic_cast<const ConvertToExpr *>(&expression))
    {
        std::unique_ptr<Expr> newVal = convert->valueExpr
                                           ? cloneExpressionReplacingParamsWithArgs(
                                                 *convert->valueExpr, argumentValues, objectExpr, classAttributes)
                                           : nullptr;

        return std::make_unique<ConvertToExpr>(std::move(newVal), convert->targetType);
    }

    if (auto mc = dynamic_cast<const MethodCallExpr *>(&expression))
    {
        std::unique_ptr<Expr> newObj =
            mc->object
                ? cloneExpressionReplacingParamsWithArgs(*mc->object, argumentValues, objectExpr, classAttributes)
                : nullptr;
        std::vector<std::unique_ptr<Expr>> newArgs;
        for (const auto &a : mc->args)
            if (a)
                newArgs.push_back(
                    cloneExpressionReplacingParamsWithArgs(*a, argumentValues, objectExpr, classAttributes));

        return std::make_unique<MethodCallExpr>(std::move(newObj), mc->method, std::move(newArgs), mc->interpreter);
    }

    return nullptr;
}
