#include "method_call_expr.hpp"
#include "interpreter/expr_visitor.hpp"
#include "interpreter/interpreter.hpp"
#include "interpreter/runtime/method.hpp"
#include "ast/stmt/create_method_stmt.hpp"

Value MethodCallExpr::accept(ExprVisitor &visitor) const
{
    return visitor.visitMethodCallExpr(*this);
}

Value MethodCallExpr::evaluate(std::shared_ptr<Heart> heart) const
{
    return interpreter->visitMethodCallExpr(*this);
}

std::shared_ptr<CreationType> MethodCallExpr::getReturnType(ParserContext &ctx) const
{
    return method->getReturnType(ctx);
};

bool MethodCallExpr::isArgumentAssignable(const std::shared_ptr<CreationType> &paramType, const std::shared_ptr<CreationType> &argType) const
{
    if (!paramType || !argType)
        return false;

    if (argType->isA(paramType) || paramType->isCompatibleWith(argType))
        return true;

    if (argType->isPolymorphic() && argType->parent_class->isA(paramType))
        return true;

    return false;
}

void MethodCallExpr::validate(ParserContext &ctx) const
{
    if (method && method->name == "__alpha__")
    {
        throw std::runtime_error("Constructor '__alpha__' cannot be called directly.");
    }

    if (method && method->name == "__omega__")
    {
        throw std::runtime_error("Destructor '__omega__' cannot be called directly.");
    }

    const auto &paramNames = method->params->getVariableNames();
    const auto &paramsCount = method->params->paramsCount;

    // -------------------------------------------------------------
    // Parameters carrying a default value may be omitted by the
    // caller; the default is substituted at runtime.
    // -------------------------------------------------------------
    std::vector<bool> hasDefault(paramsCount, false);
    {
        auto userMethod = std::dynamic_pointer_cast<Method>(method);
        if (userMethod && userMethod->definition)
        {
            const auto &defaults = userMethod->definition->defaultValues;
            for (size_t i = 0; i < defaults.size() && i < paramsCount; ++i)
                hasDefault[i] = defaults[i] != nullptr;
        }
    }

    size_t requiredCount = 0;
    for (size_t i = 0; i < paramsCount; ++i)
        if (!hasDefault[i])
            ++requiredCount;

    if (args.size() < requiredCount || args.size() > paramsCount)
    {
        const std::string countDescription = (requiredCount == paramsCount)
                                                 ? std::to_string(paramsCount) + " argument(s)"
                                                 : "at least " + std::to_string(requiredCount) + " argument(s)";
        std::string message = "Method '" + method->name + "' expects " + countDescription + ", but got " +
                              std::to_string(args.size()) + ".";

        if (args.size() < requiredCount)
        {
            message += "\n\nMissing parameter(s):";

            for (size_t i = 0; i < paramsCount; ++i)
            {
                if (hasDefault[i])
                    continue;

                auto paramType = method->params->getVarType(paramNames[i]);
                message += "\n - " + paramNames[i] + ": " + paramType->name;
            }
        }

        throw std::runtime_error(message);
    }

    for (size_t i = 0; i < args.size(); ++i)
    {
        auto argType = args[i]->getReturnType(ctx);

        const size_t paramIdx = (!argIndices.empty() && i < argIndices.size()) ? argIndices[i] : i;
        if (paramIdx >= paramsCount)
            continue;

        auto paramType = method->params->getVarType(paramNames[paramIdx]);

        if (!isArgumentAssignable(paramType, argType))
        {
            throw std::runtime_error(
                "Argument '" + paramNames[paramIdx] +
                "' for method '" + method->name +
                "' expects type '" + paramType->name +
                "', but got '" + argType->name + "'.");
        }
    }
}
