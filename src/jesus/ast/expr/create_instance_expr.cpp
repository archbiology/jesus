#include "create_instance_expr.hpp"
#include "list_expr.hpp"
#include "interpreter/expr_visitor.hpp"
#include "interpreter/runtime/method.hpp"
#include "parser/helpers/member.hpp"
#include "ast/stmt/create_method_stmt.hpp"

Value CreateInstanceExpr::accept(ExprVisitor &visitor) const { return visitor.visitCreateInstanceExpr(*this); }

void CreateInstanceExpr::validate(ParserContext &ctx) const
{
    // ---------------------------------------------------------------
    // Find the class constructor.
    //
    // A class without an __alpha__ constructor accepts no constructor
    // arguments, just like a constructor with zero parameters.
    // ---------------------------------------------------------------
    std::shared_ptr<IMethod> constructor = nullptr;
    auto __alpha__ = klass->findMember("__alpha__", klass);
    if (__alpha__ && __alpha__->isMethod())
        constructor = __alpha__->method;

    const size_t paramsCount = constructor ? constructor->params->paramsCount : 0;
    const auto paramNames = constructor ? constructor->params->getVariableNames() : std::vector<std::string>{};

    // ------------------------------------------------------------------
    // Extract the arguments passed to the constructor.
    //
    // Constructor arguments are represented as a ListExpr when multiple
    // arguments are present, but a single argument may be represented
    // directly as an expression.
    // ------------------------------------------------------------------
    std::vector<const Expr *> arguments;
    if (constructorArgs)
    {
        if (auto list = dynamic_cast<const ListExpr *>(constructorArgs.get()))
        {
            for (const auto &arg : list->elements)
                arguments.push_back(arg.get());
        }
        else
        {
            arguments.push_back(constructorArgs.get());
        }
    }

    // -------------------------------------------------------------
    // The number of arguments must match the constructor's parameter
    // count. Parameters carrying a default value may be omitted.
    // -------------------------------------------------------------
    std::vector<bool> hasDefault(paramsCount, false);
    {
        auto methodCtor = std::dynamic_pointer_cast<Method>(constructor);
        if (methodCtor && methodCtor->definition)
        {
            const auto &defaults = methodCtor->definition->defaultValues;
            for (size_t i = 0; i < defaults.size() && i < paramsCount; ++i)
                hasDefault[i] = defaults[i] != nullptr;
        }
    }

    size_t requiredCount = 0;
    for (size_t i = 0; i < paramsCount; ++i)
        if (!hasDefault[i])
            ++requiredCount;

    if (arguments.size() < requiredCount || arguments.size() > paramsCount)
    {
        const std::string countDescription = (requiredCount == paramsCount)
                                                 ? std::to_string(paramsCount) + " parameter(s)"
                                                 : "at least " + std::to_string(requiredCount) + " parameter(s)";
        std::string message = "Constructor '" + klass->name + "' expects " + countDescription +
                              ", but got " + std::to_string(arguments.size()) + ".";

        if (arguments.size() < requiredCount)
        {
            message += "\n\nMissing parameter(s):";

            for (size_t i = 0; i < paramsCount; ++i)
            {
                if (hasDefault[i])
                    continue;

                auto paramType = constructor->params->getVarType(paramNames[i]);
                message += "\n - " + paramNames[i] + ": " + paramType->name;
            }
        }

        throw std::runtime_error(message);
    }

    // ----------------------------------------------------------------
    // Validate each argument against the corresponding parameter type.
    // ----------------------------------------------------------------
    for (size_t i = 0; i < arguments.size(); ++i)
    {
        const size_t paramIdx = (!argIndices.empty() && i < argIndices.size()) ? argIndices[i] : i;

        auto argType = arguments[i]->getReturnType(ctx);
        auto paramType = constructor->params->getVarType(paramNames[paramIdx]);

        const bool assignable = (argType->isA(paramType) || paramType->isCompatibleWith(argType)) ||
                                (argType->isPolymorphic() && argType->parent_class->isA(paramType));

        if (!assignable)
        {
            throw std::runtime_error(
                "Argument '" + paramNames[paramIdx] + "' for the '" + klass->name + "' constructor expects a '" +
                paramType->name + "', but got '" + argType->name + "'.");
        }
    }
}
