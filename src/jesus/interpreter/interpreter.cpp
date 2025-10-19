#include "interpreter.hpp"
#include <stdexcept>
#include "runtime/instance.hpp"
#include "runtime/method.hpp"
#include "../ast/stmt/create_method_stmt.hpp"
#include "../ast/stmt/print_stmt.hpp"
#include "../types/known_types.hpp"
#include "../utils/string_utils.hpp"

#include <iostream>

// Custom control-flow exceptions
class BreakSignal : public std::exception
{
};
class SkipSignal : public std::exception
{
};

Value Interpreter::evaluate(const std::unique_ptr<Expr> &expr)
{
    return expr->accept(*this);
}

void Interpreter::execute(const std::unique_ptr<Stmt> &stmt)
{
    stmt->accept(*this);
}

void Interpreter::execute(const std::shared_ptr<Stmt> &stmt)
{
    stmt->accept(*this);
}

void Interpreter::createVariable(const std::string &type, const std::string &name, const Value &value)
{
    symbol_table.createVar(type, name, value);
}

void Interpreter::updateVariable(const std::string &name, const Value &value)
{
    symbol_table.updateVar(name, value);
}

Value Interpreter::visitBinary(const BinaryExpr &expr)
{
    return expr.evaluate(symbol_table.currentScope());
}

Value Interpreter::visitUnary(const UnaryExpr &expr)
{
    return expr.evaluate(symbol_table.currentScope());
}

Value Interpreter::visitLiteral(const LiteralExpr &expr)
{
    return expr.value;
}

Value Interpreter::visitVariable(const VariableExpr &expr)
{
    return symbol_table.getVar(expr.name);
}

Value Interpreter::visitCreateInstanceExpr(const CreateInstanceExpr &expr)
{
    auto instance = std::make_shared<Instance>(expr.klass);
    return Value(instance);
}

Value Interpreter::visitGetAttribute(const GetAttributeExpr &expr)
{
    Value obj = expr.object->accept(*this);

    std::shared_ptr<Instance> instance = obj.toInstance();

    return instance->getAttribute(expr.attribute);
}

Value Interpreter::visitParityCheckExpr(const ParityCheckExpr &expr)
{
    return expr.evaluate(symbol_table.currentScope());
}

Value Interpreter::visitMethodCallExpr(const MethodCallExpr &expr)
{
    Value object = expr.object->accept(*this);

    std::shared_ptr<Instance> instance = object.toInstance();
    std::vector<Value> args;

    // Evaluate all arguments
    for (auto &argExpr : expr.args)
        args.push_back(argExpr->accept(*this));

    return expr.method->call(*this, instance, std::move(args));
}

Value Interpreter::visitFormattedStringExpr(const FormattedStringExpr &expr)
{
    std::string result;

    for (size_t i = 0; i < expr.parts.size(); ++i)
    {
        result += expr.parts[i];
        if (i < expr.expressions.size())
        {
            Value val = expr.expressions[i]->accept(*this);
            result += val.toString();
        }
    }

    return Value(result);
}

Value Interpreter::visitAsk(const AskExpr &expr)
{
    Value question = expr.prompt->accept(*this);
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
    createVariable(stmt.base_type, stmt.name, val);
}

Value Interpreter::askAndValidate(const std::shared_ptr<Expr> ask_expr, std::shared_ptr<CreationType> var_type)
{
    // Step 1: Evaluate the ask expression (e.g., ask "Your age?")
    Value question = ask_expr->accept(*this);

    while (true)
    {
        // Step 2: Show the question to the user
        std::cout << question.toString();

        // Step 3: Get the answer from the user
        std::string answer;
        std::getline(std::cin, answer);
        answer = utils::trim(answer);

        try
        {
            // Step 4: Validate the value according to the variable type
            Value value = var_type->parseFromString(answer);
            var_type->validate(value);

            // Step 5: return validated value
            return value;
        }
        catch (const std::exception &e)
        {
            std::cerr << "Validation failed: " << e.what() << std::endl;
        }
    }
}

void Interpreter::visitCreateVarWithAsk(const CreateVarWithAskStmt &stmt)
{
    Value value = askAndValidate(stmt.ask_expr, stmt.var_type);
    createVariable(stmt.var_type->name, stmt.var_name, value);
}

void Interpreter::visitUpdateVarWithAsk(const UpdateVarWithAskStmt &stmt)
{
    Value value = askAndValidate(stmt.ask_expr, stmt.var_type);
    updateVariable(stmt.var_name, value);
}

void Interpreter::visitCreateClass(const CreateClassStmt &stmt)
{
    std::vector<std::shared_ptr<IConstraint>> constraints; // no constraints yet

    auto userClass = std::make_shared<CreationType>(
        PrimitiveType::Class,
        stmt.name,
        stmt.module_name,
        stmt.parent_class,
        constraints);

    for (auto &member : stmt.body)
    {
        // -----------------
        // handle attributes
        // -----------------
        if (auto attr = dynamic_cast<CreateVarStmt *>(member.get()))
        {
            userClass->addAttribute(attr->base_type, attr->name, std::move(attr->value), symbol_table.currentScope());
        }
        // --------------
        // handle methods
        // --------------
        else if (auto methodStmt = dynamic_cast<CreateMethodStmt *>(member.get()))
        {
            auto method = std::make_shared<Method>(
                methodStmt->name,
                methodStmt->params,
                methodStmt->body,
                methodStmt->returnType);

            userClass->addMethod(methodStmt->name, method);
        }
    }

    KnownTypes::registerType(std::move(userClass));
}

void Interpreter::visitCreateVarType(const CreateVarTypeStmt &stmt)
{
    auto custom_type = std::make_shared<CreationType>(
        stmt.base_type->primitive_type,
        stmt.name,
        stmt.module_name,
        stmt.base_type,
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

void Interpreter::visitPrintStmt(const PrintStmt &stmt)
{
    Value value = evaluate(stmt.message);

    std::ostream &out = (stmt.type == StmtType::WARN) ? std::cerr : std::cout;

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
        catch (const SkipSignal &)
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
        catch (const SkipSignal &)
        {
            continue;
        }
        catch (const BreakSignal &)
        {
            break;
        }
    }
}

void Interpreter::visitRepeatForeverStmt(const RepeatForeverStmt &stmt)
{
    while (true)
    {
        try
        {
            for (const auto &statement : stmt.body)
            {
                execute(statement);
            }
        }
        catch (const SkipSignal &)
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
        // heart.createVar(stmt.varName, *value); // like "let name = item" // TODO: Use a pointer instead of *value

        try
        {
            for (auto &statement : stmt.body)
            {
                execute(statement);
            }
        }
        catch (const SkipSignal &)
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

void Interpreter::visitSkipStmt(const SkipStmt &)
{
    throw SkipSignal();
}

void Interpreter::visitReturnStmt(const ReturnStmt &stmt)
{
    Value result;

    if (stmt.value)
    {
        result = evaluate(stmt.value);
    }

    throw ReturnSignal(result); // return execution flow to caller, Method::call.
}

void Interpreter::visitIfStmt(const IfStmt &stmt)
{
    Value conditionValue = evaluate(stmt.condition);

    if (conditionValue.AS_BOOLEAN)
    {
        for (auto &stmt : stmt.thenBranch)
            execute(stmt);
    }
    else
    {
        for (auto &stmt : stmt.otherwiseBranch)
            execute(stmt);
    }
}
