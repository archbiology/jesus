#include "method_call_expr.hpp"
#include "../../interpreter/expr_visitor.hpp"
#include "../../interpreter/runtime/method.hpp"

Value MethodCallExpr::accept(ExprVisitor &visitor) const
{
    return visitor.visitMethodCallExpr(*this);
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
    const auto &paramNames = method->params->getVariableNames();

    if (args.size() != paramNames.size())
    {
        std::string message =
            "Method '" + method->name +
            "' expects " + std::to_string(paramNames.size()) +
            " argument(s), but got " + std::to_string(args.size()) + ".";

        if (args.size() < paramNames.size())
        {
            message += "\n\nMissing parameter(s):";

            for (size_t i = args.size(); i < paramNames.size(); ++i)
            {
                auto paramType = method->params->getVarType(paramNames[i]);
                message += "\n - " + paramNames[i] + ": " + paramType->name;
            }
        }

        throw std::runtime_error(message);
    }

    for (size_t i = 0; i < args.size(); ++i)
    {
        auto argType = args[i]->getReturnType(ctx);

        auto paramType =
            method->params->getVarType(paramNames[i]);

        if (!isArgumentAssignable(paramType, argType))
        {
            throw std::runtime_error(
                "Argument '" + paramNames[i] +
                "' for method '" + method->name +
                "' expects type '" + paramType->name +
                "', but got '" + argType->name + "'.");
        }
    }
}
