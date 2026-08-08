#include "loop_unroller.hpp"

#include "ast/stmt/repeat_while_stmt.hpp"
#include "ast/stmt/repeat_times_stmt.hpp"
#include "ast/stmt/repeat_forever_stmt.hpp"
#include "ast/stmt/for_each_stmt.hpp"
#include "ast/stmt/print_stmt.hpp"
#include "ast/stmt/create_var_stmt.hpp"
#include "ast/stmt/update_var_stmt.hpp"
#include "ast/stmt/assign_stmt.hpp"
#include "ast/stmt/create_var_with_ask_stmt.hpp"
#include "ast/stmt/update_var_with_ask_stmt.hpp"
#include "ast/stmt/inspect_stmt.hpp"
#include "ast/stmt/memory_inspect_stmt.hpp"
#include "ast/stmt/import_module_stmt.hpp"
#include "ast/stmt/serve_stmt.hpp"
#include "ast/stmt/create_var_type_stmt.hpp"

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
#include "ast/expr/bible_expr.hpp"
#include "ast/expr/parity_check_expr.hpp"
#include "ast/expr/ask_expr.hpp"
#include "ast/expr/create_instance_expr.hpp"
#include "ast/expr/convert_to_expr.hpp"
#include "ast/expr/assignable_expr.hpp"

void LoopUnroller::run(std::vector<std::unique_ptr<Stmt>> &program) { unrollBlock(program); }

void LoopUnroller::unrollBlock(std::vector<std::unique_ptr<Stmt>> &stmts)
{
    std::vector<std::unique_ptr<Stmt>> result;
    result.reserve(stmts.size());

    for (auto &stmt : stmts)
    {
        if (!stmt)
            continue;

        auto replacements = unrollStatement(std::move(stmt));

        for (auto &replacement : replacements)
        {
            if (replacement)
                result.push_back(std::move(replacement));
        }
    }

    stmts = std::move(result);
}

std::vector<std::unique_ptr<Stmt>> LoopUnroller::unrollStatement(std::unique_ptr<Stmt> statement)
{
    std::vector<std::unique_ptr<Stmt>> result;

    if (!statement)
        return result;

    // ---------------
    // repeat N times
    // ---------------
    if (auto repeatTimes = dynamic_cast<RepeatTimesStmt *>(statement.get()))
    {
        if (shouldUnroll(*repeatTimes))
        {
            const int count = literalCount(repeatTimes->countExpr.get());

            for (int i = 0; i < count; ++i)
            {
                for (const auto &child : repeatTimes->body)
                {
                    if (child)
                        result.push_back(cloneStatement(*child));
                }
            }

            return result;
        }

        // ----------------------------------------
        // We couldn't unroll this loop.
        // Let's inspect its body for nested loops.
        // ----------------------------------------
        unrollBlock(repeatTimes->body);

        result.push_back(std::move(statement));
        return result;
    }

    // -------------
    // repeat while
    // -------------
    if (auto repeatWhile = dynamic_cast<RepeatWhileStmt *>(statement.get()))
    {
        unrollBlock(repeatWhile->body);

        result.push_back(std::move(statement));
        return result;
    }

    // ---------------
    // repeat forever
    // ---------------
    if (auto repeatForever = dynamic_cast<RepeatForeverStmt *>(statement.get()))
    {
        unrollBlock(repeatForever->body);

        result.push_back(std::move(statement));
        return result;
    }

    // ---------
    // for each
    // ---------
    if (auto forEach = dynamic_cast<ForEachStmt *>(statement.get()))
    {
        unrollBlock(forEach->body);

        result.push_back(std::move(statement));
        return result;
    }

    // The statement was not a loop. Let's just return it.
    result.push_back(std::move(statement));
    return result;
}

int LoopUnroller::literalCount(const Expr *countExpr) const
{
    auto literal = dynamic_cast<const LiteralExpr *>(countExpr);
    if (!literal)
        return -1;

    const Value &value = literal->value;
    if (!value.IS_INT)
        return -1;

    return value.toInt();
}

bool LoopUnroller::shouldUnroll(const RepeatTimesStmt &loop) const
{
    const int count = literalCount(loop.countExpr.get());

    if (count <= 0)
        return false;

    if (count > MAX_UNROLL_COUNT)
        return false;

    if (loop.body.size() > MAX_UNROLL_BODY_STATEMENTS)
        return false;

    for (const auto &child : loop.body)
    {
        if (!child)
            continue;

        // FIXME: calling cloneStatement here is expensive, because it's realy usefulness is inside unrollStatement.
        if (!cloneStatement(*child))
            return false;
    }

    return true;
}

std::unique_ptr<Stmt> LoopUnroller::cloneStatement(const Stmt &statement) const
{
    if (auto print = dynamic_cast<const PrintStmt *>(&statement))
    {
        return std::make_unique<PrintStmt>(print->type, print->message ? cloneExpression(*print->message) : nullptr);
    }

    if (auto create = dynamic_cast<const CreateVarStmt *>(&statement))
    {
        return std::make_unique<CreateVarStmt>(
            create->name, create->address, create->value ? cloneExpression(*create->value) : nullptr);
    }

    if (auto update = dynamic_cast<const UpdateVarStmt *>(&statement))
    {
        return std::make_unique<UpdateVarStmt>(
            update->name, update->address, update->value ? cloneExpression(*update->value) : nullptr);
    }

    if (auto assign = dynamic_cast<const AssignStmt *>(&statement))
    {
        return std::make_unique<AssignStmt>(
            assign->target ? cloneAssignableExpr(*assign->target) : nullptr,
            assign->value ? cloneExpression(*assign->value) : nullptr);
    }

    if (auto createAsk = dynamic_cast<const CreateVarWithAskStmt *>(&statement))
    {
        return std::make_unique<CreateVarWithAskStmt>(
            createAsk->var_type,
            createAsk->var_name,
            createAsk->address,
            createAsk->ask_expr ? cloneExpression(*createAsk->ask_expr) : nullptr);
    }

    if (auto updateAsk = dynamic_cast<const UpdateVarWithAskStmt *>(&statement))
    {
        return std::make_unique<UpdateVarWithAskStmt>(
            updateAsk->var_type,
            updateAsk->var_name,
            updateAsk->address,
            updateAsk->ask_expr ? cloneExpression(*updateAsk->ask_expr) : nullptr);
    }

    if (auto inspect = dynamic_cast<const InspectStmt *>(&statement))
        return std::make_unique<InspectStmt>(inspect->target, inspect->symbolName);

    if (dynamic_cast<const MemoryInspectStmt *>(&statement))
        return std::make_unique<MemoryInspectStmt>();

    if (auto import = dynamic_cast<const ImportModuleStmt *>(&statement))
    {
        return std::make_unique<ImportModuleStmt>(
            import->relativeDepth, import->modules, import->moduleAlias, import->importedSymbols);
    }

    if (auto serve = dynamic_cast<const ServeStmt *>(&statement))
    {
        return std::make_unique<ServeStmt>(serve->portExpr ? cloneExpression(*serve->portExpr) : nullptr);
    }

    if (auto varType = dynamic_cast<const CreateVarTypeStmt *>(&statement))
    {
        return std::make_unique<CreateVarTypeStmt>(
            varType->base_type, varType->name, varType->module_name, varType->constraints);
    }

    // -------------------------------------------------------------------
    // The following statements are intentionally NOT cloned, which makes
    // the loop body non-unrollable:
    //
    //   - BreakStmt / SkipStmt: unrolling would change loop control flow.
    //   - ReturnStmt / ResistStmt: conservative safety.
    //   - IfStmt, RepeatWhileStmt, RepeatTimesStmt, RepeatForeverStmt,
    //     ForEachStmt, CreateClassStmt, CreateMethodStmt, TryStmt, ...
    //     statements that contain nested blocks are left alone.
    // -------------------------------------------------------------------
    return nullptr;
}

std::unique_ptr<AssignableExpr> LoopUnroller::cloneAssignableExpr(const AssignableExpr &target) const
{
    if (auto indexExpr = dynamic_cast<const IndexExpr *>(&target))
    {
        return std::make_unique<IndexExpr>(
            indexExpr->collection ? cloneExpression(*indexExpr->collection) : nullptr,
            indexExpr->index ? cloneExpression(*indexExpr->index) : nullptr);
    }

    if (auto getAttr = dynamic_cast<const GetAttributeExpr *>(&target))
    {
        return std::make_unique<GetAttributeExpr>(
            getAttr->object ? cloneExpression(*getAttr->object) : nullptr, getAttr->attribute, getAttr->address);
    }

    return nullptr;
}

std::unique_ptr<Expr> LoopUnroller::cloneExpression(const Expr &expression) const
{
    if (auto literal = dynamic_cast<const LiteralExpr *>(&expression))
        return std::make_unique<LiteralExpr>(literal->value, literal->type);

    if (auto variable = dynamic_cast<const VariableExpr *>(&expression))
        return std::make_unique<VariableExpr>(variable->address, variable->name);

    if (auto grouping = dynamic_cast<const GroupingExpr *>(&expression))
        return std::make_unique<GroupingExpr>(grouping->expression ? cloneExpression(*grouping->expression) : nullptr);

    if (auto unary = dynamic_cast<const UnaryExpr *>(&expression))
        return std::make_unique<UnaryExpr>(unary->op, unary->right ? cloneExpression(*unary->right) : nullptr);

    if (auto binary = dynamic_cast<const BinaryExpr *>(&expression))
        return std::make_unique<BinaryExpr>(
            binary->left ? cloneExpression(*binary->left) : nullptr,
            binary->op,
            binary->right ? cloneExpression(*binary->right) : nullptr);

    if (auto listExpr = dynamic_cast<const ListExpr *>(&expression))
    {
        std::vector<std::unique_ptr<Expr>> elements;
        for (const auto &element : listExpr->elements)
            if (element)
                elements.push_back(cloneExpression(*element));

        return std::make_unique<ListExpr>(std::move(elements), listExpr->listType);
    }

    if (auto dictExpr = dynamic_cast<const DictExpr *>(&expression))
    {
        std::vector<std::pair<std::unique_ptr<Expr>, std::unique_ptr<Expr>>> entries;
        for (const auto &[key, value] : dictExpr->entries)
        {
            entries.push_back({
                key ? cloneExpression(*key) : nullptr,
                value ? cloneExpression(*value) : nullptr,
            });
        }

        return std::make_unique<DictExpr>(std::move(entries), dictExpr->dictType);
    }

    if (auto condExpr = dynamic_cast<const ConditionalExpr *>(&expression))
        return std::make_unique<ConditionalExpr>(
            condExpr->condition ? cloneExpression(*condExpr->condition) : nullptr,
            condExpr->thenBranch ? cloneExpression(*condExpr->thenBranch) : nullptr,
            condExpr->elseBranch ? cloneExpression(*condExpr->elseBranch) : nullptr);

    if (auto fmtString = dynamic_cast<const FormattedStringExpr *>(&expression))
    {
        std::vector<std::unique_ptr<Expr>> expressions;

        for (const auto &expr : fmtString->expressions)
            if (expr)
                expressions.push_back(cloneExpression(*expr));

        return std::make_unique<FormattedStringExpr>(fmtString->raw, fmtString->parts, std::move(expressions));
    }

    if (auto methodCall = dynamic_cast<const MethodCallExpr *>(&expression))
    {
        std::vector<std::unique_ptr<Expr>> args;
        for (const auto &arg : methodCall->args)
            if (arg)
                args.push_back(cloneExpression(*arg));

        return std::make_unique<MethodCallExpr>(
            methodCall->object ? cloneExpression(*methodCall->object) : nullptr,
            methodCall->method,
            std::move(args),
            methodCall->interpreter);
    }

    if (auto getAttr = dynamic_cast<const GetAttributeExpr *>(&expression))
        return std::make_unique<GetAttributeExpr>(
            getAttr->object ? cloneExpression(*getAttr->object) : nullptr, getAttr->attribute, getAttr->address);

    if (auto indexExpr = dynamic_cast<const IndexExpr *>(&expression))
        return std::make_unique<IndexExpr>(
            indexExpr->collection ? cloneExpression(*indexExpr->collection) : nullptr,
            indexExpr->index ? cloneExpression(*indexExpr->index) : nullptr);

    if (auto bibleExpr = dynamic_cast<const BibleExpr *>(&expression))
        return std::make_unique<BibleExpr>(bibleExpr->reference ? cloneExpression(*bibleExpr->reference) : nullptr);

    if (auto parity = dynamic_cast<const ParityCheckExpr *>(&expression))
        return std::make_unique<ParityCheckExpr>(
            parity->target ? cloneExpression(*parity->target) : nullptr, parity->negate, parity->checkOdd);

    if (auto ask = dynamic_cast<const AskExpr *>(&expression))
        return std::make_unique<AskExpr>(ask->prompt ? cloneExpression(*ask->prompt) : nullptr);

    if (auto createInst = dynamic_cast<const CreateInstanceExpr *>(&expression))
        return std::make_unique<CreateInstanceExpr>(
            createInst->name,
            createInst->klass,
            createInst->constructorArgs ? cloneExpression(*createInst->constructorArgs) : nullptr);

    if (auto convert = dynamic_cast<const ConvertToExpr *>(&expression))
        return std::make_unique<ConvertToExpr>(
            convert->valueExpr ? cloneExpression(*convert->valueExpr) : nullptr, convert->targetType);

    return nullptr;
}
