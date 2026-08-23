#pragma once
#include <memory>
#include <optional>
#include <stdexcept>
#include <string>
#include <vector>
#include "ast/expr/expr.hpp"
#include "spirit/heart.hpp"

/**
 * @brief Orders call arguments (positional and named) against a
 * parameter list, following Python's calling conventions.
 *
 * Positional arguments fill the next unbound parameter; named
 * arguments (name=value) fill the parameter with the matching name.
 * Named arguments may appear in any order, but once a named argument
 * appears the remaining arguments must also be named.
 *
 * Used by both constructors and method calls so their semantics
 * and error messages stay in sync.
 */
namespace grammar
{
struct Argument
{
    std::optional<std::string> name;
    std::unique_ptr<Expr> value;
};

/**
 * @brief Binds arguments to parameters in parameter declaration order.
 *
 * @param args            Arguments exactly as written by the caller.
 * @param parameterNames  Parameter names, in declaration order.
 * @param parameterScope  The parameter scope, used for types in error messages.
 * @param kind            "Constructor" or "Method" (for error messages).
 * @param target           The class or method name (for error messages).
 *
 * @return Argument expressions in parameter declaration order.
 *
 * @throws std::runtime_error on unknown parameter names, duplicated
 *         named arguments, positional arguments after named arguments,
 *         or an incorrect number of arguments.
 */
inline std::vector<std::unique_ptr<Expr>> bindArgumentsToParameters(
    std::vector<Argument> &&args,
    const std::vector<std::string> &parameterNames,
    const std::shared_ptr<Heart> &parameterScope,
    const std::string &kind,
    const std::string &target)
{
    const bool isMethod = (kind == "Method");
    const std::string countWord = isMethod ? "argument(s)" : "parameter(s)";
    const std::string what = isMethod ? "method" : "constructor";

    const size_t parameterCount = parameterNames.size();

    std::vector<std::unique_ptr<Expr>> orderedArguments(parameterCount);
    std::vector<bool> parameterBound(parameterCount, false);

    size_t nextUnboundParameter = 0;
    bool namedArgumentsStarted = false;
    size_t boundArgumentCount = 0;

    auto arityError = [&](size_t gotCount)
    {
        std::string message = kind + " '" + target + "' expects " + std::to_string(parameterCount) + " " + countWord +
                              ", but got " + std::to_string(gotCount) + ".";

        if (gotCount < parameterCount)
        {
            message += "\n\nMissing parameter(s):";

            for (size_t i = 0; i < parameterCount; ++i)
            {
                if (!parameterBound[i])
                {
                    message += "\n - " + parameterNames[i] + ": " +
                               (parameterScope ? parameterScope->getVarType(parameterNames[i])->name : "unknown");
                }
            }
        }

        return message;
    };

    for (auto &arg : args)
    {
        if (arg.name.has_value())
        {
            namedArgumentsStarted = true;

            size_t idx = parameterCount;
            for (size_t i = 0; i < parameterCount; ++i)
            {
                if (parameterNames[i] == *arg.name)
                {
                    idx = i;
                    break;
                }
            }

            if (idx == parameterCount)
            {
                throw std::runtime_error(kind + " '" + target + "' has no parameter named '" + *arg.name + "'.");
            }

            if (parameterBound[idx])
            {
                throw std::runtime_error("Multiple values given for argument '" + *arg.name + "'.");
            }

            orderedArguments[idx] = std::move(arg.value);
            parameterBound[idx] = true;
            ++boundArgumentCount;
        }
        else
        {
            if (namedArgumentsStarted)
            {
                throw std::runtime_error(
                    "Positional argument cannot follow a named argument in " + what + " '" + target + "'.");
            }

            // -----------------------------------------------------------------------------
            // Find the first parameter that has not already been bound by a named argument.
            // -----------------------------------------------------------------------------
            while (nextUnboundParameter < parameterCount && parameterBound[nextUnboundParameter])
                ++nextUnboundParameter;

            if (nextUnboundParameter >= parameterCount)
            {
                throw std::runtime_error(arityError(args.size()));
            }

            orderedArguments[nextUnboundParameter] = std::move(arg.value);
            parameterBound[nextUnboundParameter] = true;
            ++boundArgumentCount;
        }
    }

    if (boundArgumentCount != parameterCount)
    {
        throw std::runtime_error(arityError(boundArgumentCount));
    }

    return orderedArguments;
}

} // namespace grammar
