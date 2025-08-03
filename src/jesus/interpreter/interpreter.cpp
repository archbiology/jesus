#include "interpreter.hpp"
#include <stdexcept>
#include <csignal>
#include "../ast/stmt/output_statement.hpp"
#include "../types/known_types.hpp"
#include "../utils/string_utils.hpp"

#include <iostream>

// Custom control-flow exceptions
class BreakSignal : public std::exception
{
};
class ContinueSignal : public std::exception
{
};

void Interpreter::setSignalHandler()
{
    std::signal(SIGINT, signalHandler); // SIGINT = CTRL+C
}

/**
 * Creating the Interpreter::instance variable.
 */
Interpreter *Interpreter::instance = nullptr;

void Interpreter::signalHandler(int signum)
{
    std::cout << "\nCaught signal " << signum << ". Exiting input loop..." << std::endl;

    if (Interpreter::instance)
    {
        Interpreter::instance->keep_running.store(false);
    }
}

Value Interpreter::evaluate(const std::unique_ptr<Expr> &expr)
{
    return expr->accept(*this);
}

void Interpreter::execute(const std::unique_ptr<Stmt> &stmt)
{
    stmt->accept(*this);
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

Value Interpreter::visitAsk(const AskExpr &expr)
{
    auto question = expr.evaluate(&heart);
    return question;
}

Value Interpreter::visitGrouping(const GroupingExpr &expr)
{
    return evaluate(expr.expression);
}

std::string Interpreter::valueToString(const Value &value)
{
    return value.toString();
}

void Interpreter::visitCreateVar(const CreateVarStmt &stmt)
{
    Value val = evaluate(stmt.value);
    createVariable(stmt.name, val);
}

void Interpreter::visitCreateVarWithAsk(const CreateVarWithAskStmt &stmt)
{
    // Step 1: Evaluate the ask expression (e.g., ask "Your age?")
    Value question = stmt.ask_expr->evaluate(&heart);

    while (keep_running)
    {
        // Step 2: Show the question to the user
        std::cout << question.toString();

        // Step 3: Get the answer from the user
        std::string answer;
        std::getline(std::cin, answer);

        if (! keep_running) {
            std::cerr << "\n^C caught. Input aborted. Try again.\n";
            interrupted = 0;
            input.clear();
            // print prompt again
        }

        try
        {
            answer = utils::trim(answer);

            // Step 4: Validate the value according to the creation type
            Value value = stmt.var_type.parseFromString(answer);
            stmt.var_type.validate(value);

            // Step 5: Only if validation passed, create the variable
            createVariable(stmt.var_name, value);
            break;
        }
        catch (const std::exception &e)
        {
            std::cerr << "Validation failed: " << e.what() << std::endl;
        }
    }

}

void Interpreter::visitCreateVarType(const CreateVarTypeStmt &stmt)
{
    auto custom_type = std::make_shared<CreationType>(
        stmt.base_type.primitive_type,
        stmt.name,
        stmt.module_name,
        stmt.constraints);

    KnownTypes::registerType(std::move(custom_type));
}

void Interpreter::visitUpdateVar(const UpdateVarStmt &stmt)
{
    Value val = evaluate(stmt.value);
    updateVariable(stmt.name, val);
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

void Interpreter::visitOutput(const OutputStmt &stmt)
{
    Value value = evaluate(stmt.message);

    std::ostream &out = (stmt.type == OutputType::WARN) ? std::cerr : std::cout;

    out << value.toString() << std::endl;
}

void Interpreter::visitRepeatWhile(const RepeatWhileStmt &stmt)
{
    while (evaluate(stmt.condition).AS_BOOLEAN)
    {
        try
        {
            for (const auto &statement : stmt.body)
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

void Interpreter::visitRepeatTimes(const RepeatTimesStmt &stmt)
{
    Value countVal = evaluate(stmt.countExpr);
    if (!countVal.IS_INT)
    {
        throw std::runtime_error("repeat times expects a numeric value.");
    }

    int times = countVal.toInt();

    for (int i = 0; i < times; ++i)
    {
        try
        {
            for (const auto &statement : stmt.body)
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

void Interpreter::visitForEach(const ForEachStmt &stmt)
{
    Value listValue = evaluate(stmt.iterable);
    if (!listValue.IS_LIST)
    {
        throw std::runtime_error("Expected a list to iterate over.");
    }

    auto &items = listValue.asList();

    for (const std::shared_ptr<Value> &value : items)
    {

        // FIXME: Remove the line below.
        heart.createVar(stmt.varName, *value); // like "let name = item" // TODO: Use a pointer instead of *value

        try
        {
            for (auto &statement : stmt.body)
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

void Interpreter::visitBreak(const BreakStmt &)
{
    throw BreakSignal();
}

void Interpreter::visitContinue(const ContinueStmt &)
{
    throw ContinueSignal();
}
