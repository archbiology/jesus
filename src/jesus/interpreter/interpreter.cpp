#include "interpreter.hpp"
#include <stdexcept>
#include "../ast/stmt/output_statement.hpp"
#include <iostream>

// Custom control-flow exceptions
class BreakSignal : public std::exception
{
};
class ContinueSignal : public std::exception
{
};

Value Interpreter::evaluate(const std::unique_ptr<Expr> &expr)
{
    return expr->accept(*this);
}

void Interpreter::createVariable(const std::string &name, const Value &value)
{
    heart.createVar(name, value);
}

void Interpreter::updateVariable(const std::string &name, const Value &value)
{
    heart.updateVar(name, value);
}

Value Interpreter::visitBinary(const BinaryExpr &expr)
{
    return expr.evaluate(&heart);
}

Value Interpreter::visitUnary(const UnaryExpr &expr)
{
    return expr.evaluate(&heart);
}

Value Interpreter::visitLiteral(const LiteralExpr &expr)
{
    return expr.value;
}

Value Interpreter::visitVariable(const VariableExpr &expr)
{
    return heart.getVar(expr.name);
}

Value Interpreter::visitGrouping(const GroupingExpr &expr)
{
    return evaluate(expr.expression);
}

std::string Interpreter::valueToString(const Value &value)
{
    return value.toString();
}

void Interpreter::execute(const std::unique_ptr<Stmt> &stmt)
{
    if (auto create = dynamic_cast<CreateVarStmt *>(stmt.get()))
        return visitCreateVar(create);

    if (auto update = dynamic_cast<UpdateVarStmt *>(stmt.get()))
        return visitUpdateVar(update);

    if (auto out = dynamic_cast<OutputStmt *>(stmt.get()))
        return visitOutput(out);

    if (auto loop = dynamic_cast<RepeatWhileStmt *>(stmt.get()))
        return visitRepeatWhile(loop);

    if (auto loopTimes = dynamic_cast<RepeatTimesStmt *>(stmt.get()))
        return visitRepeatTimes(loopTimes);

    if (auto foreachStmt = dynamic_cast<ForEachStmt *>(stmt.get()))
        return visitForEach(foreachStmt);

    if (auto br = dynamic_cast<BreakStmt *>(stmt.get()))
        return visitBreak(br);

    if (auto cont = dynamic_cast<ContinueStmt *>(stmt.get()))
        return visitContinue(cont);

    throw std::runtime_error("Unknown statement type.");
}

void Interpreter::visitCreateVar(const CreateVarStmt *stmt)
{
    Value val = evaluate(stmt->value);
    createVariable(stmt->name, val);
}

void Interpreter::visitUpdateVar(const UpdateVarStmt *stmt)
{
    Value val = evaluate(stmt->value);
    updateVariable(stmt->name, val);
}

Value Interpreter::visitConditional(const ConditionalExpr &expr)
{
    Value conditionValue = evaluate(expr.condition);

    // Interpret the condition as boolean
    if (conditionValue.AS_BOOLEAN)
    {
        return evaluate(expr.thenBranch);
    }

    return evaluate(expr.elseBranch);
}

void Interpreter::visitOutput(OutputStmt *stmt)
{
    Value value = evaluate(stmt->message);

    std::ostream &out = (stmt->type == OutputType::WARN) ? std::cerr : std::cout;

    out << value.toString() << std::endl;
}

void Interpreter::visitRepeatWhile(const RepeatWhileStmt *stmt)
{
    while (evaluate(stmt->condition).AS_BOOLEAN)
    {
        try
        {
            for (const auto &statement : stmt->body)
            {
                execute(statement);
            }
        }
        catch (const ContinueSignal &)
        {
            continue;
        }
        catch (const BreakSignal &)
        {
            break;
        }
    }
}

void Interpreter::visitRepeatTimes(const RepeatTimesStmt *stmt)
{
    Value countVal = evaluate(stmt->countExpr);
    if (!countVal.IS_INT)
    {
        throw std::runtime_error("repeat times expects a numeric value.");
    }

    int times = countVal.toInt();

    for (int i = 0; i < times; ++i)
    {
        try
        {
            for (const auto &statement : stmt->body)
            {
                execute(statement);
            }
        }
        catch (const ContinueSignal &)
        {
            continue;
        }
        catch (const BreakSignal &)
        {
            break;
        }
    }
}

void Interpreter::visitForEach(ForEachStmt *stmt)
{
    Value listValue = evaluate(stmt->iterable);
    if (!listValue.IS_LIST)
    {
        throw std::runtime_error("Expected a list to iterate over.");
    }

    auto &items = listValue.asList();

    for (const std::shared_ptr<Value> &value : items)
    {

        // FIXME: Remove the line below.
        heart.createVar(stmt->varName, *value); // like "let name = item" // TODO: Use a pointer instead of *value

        try
        {
            for (auto &statement : stmt->body)
            {
                execute(statement);
            }
        }
        catch (const ContinueSignal &)
        {
            continue;
        }
        catch (const BreakSignal &)
        {
            break;
        }
    }
}

void Interpreter::visitBreak(BreakStmt *)
{
    throw BreakSignal();
}

void Interpreter::visitContinue(ContinueStmt *)
{
    throw ContinueSignal();
}
