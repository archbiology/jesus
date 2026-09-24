#include "instantiation_rule.hpp"
#include "ast/expr/create_instance_expr.hpp"
#include "ast/expr/list_expr.hpp"
#include "ast/stmt/create_method_stmt.hpp"
#include "types/known_types.hpp"
#include "parser/helpers/member.hpp"
#include "interpreter/runtime/method.hpp"
#include "parser/grammar/argument_binding.hpp"
#include "../still_parsing_multiline_arguments.hpp"

std::unique_ptr<Expr> InstantiationRule::parse(ParserContext &ctx)
{
    if (!ctx.check(TokenType::IDENTIFIER))
        return nullptr;

    int snapshot = ctx.snapshot();

    std::string className = ctx.advance().lexeme;

    if (!ctx.match(TokenType::LEFT_PAREN))
    {
        ctx.restore(snapshot);
        return nullptr;
    }

    // ----------------------------------------------------
    // Only valid, known classes can be instantiated.
    // "Make sure the class name refers to a valid class."
    // ----------------------------------------------------
    bool typeExistsLocally = ctx.varExistsInHierarchy(className);
    std::shared_ptr<CreationType> klass = nullptr;

    if (typeExistsLocally)
    {
        auto localType = ctx.getVarType(className);
        if (localType->isClass())
            klass = localType;
    }

    if (!klass)
    {
        klass = KnownTypes::resolve(className, ctx.moduleName);
        if (!klass && ctx.isClassKnown(className))
            klass = ctx.resolveType(className);
    }

    if (!klass || !klass->isClass())
    {
        throw std::runtime_error(
            "Unknown class: '" + className + "'. Make sure the class has been declared or imported.");
    }

    // ---------------------------
    // Constructor parameters.
    // A class without a defined '__alpha__' constructor behaves like
    // a constructor with zero parameters.
    // ---------------------------
    std::vector<std::string> paramNames;
    std::vector<std::shared_ptr<Expr>> defaultValues;
    std::shared_ptr<Heart> constructorScope = nullptr;
    auto __alpha__ = klass->findMember("__alpha__", klass);
    if (__alpha__ && __alpha__->isMethod())
    {
        constructorScope = __alpha__->method->params;
        paramNames = constructorScope->getParameterNames();

        if (auto constructor = std::dynamic_pointer_cast<Method>(__alpha__->method))
            if (constructor->definition)
                defaultValues = constructor->definition->defaultValues;
    }

    // ---------------------------------------------
    // Optional constructor arguments.
    // Each argument is either positional (an expression)
    // or named (name=value, as in Python).
    // ---------------------------------------------
    std::vector<grammar::Argument> rawArgs;

    if (!ctx.check(TokenType::RIGHT_PAREN))
    {
        do
        {
            // ---------------------------
            // Arguments in multiple lines
            // ---------------------------
            ctx.consumeAllNewLines();
            if (ctx.isAtEnd())
            {
                ctx.restore(snapshot);
                throw StillParsingMultilineArgumentsSignal{};
            }

            // -------------------------------------
            // Detect a named argument: name='Jesus'
            // -------------------------------------
            std::optional<std::string> argName;
            if (ctx.check(TokenType::IDENTIFIER))
            {
                int snap = ctx.snapshot();
                const Token &token = ctx.advance();
                if (ctx.check(TokenType::EQUAL))
                {
                    ctx.advance(); // consume the '='
                    argName = token.lexeme;
                }
                else
                {
                    ctx.restore(snap);
                }
            }

            auto arg = expression->parse(ctx);
            if (!arg)
            {
                throw std::runtime_error("Expected a constructor argument inside '" + className + "(...)'.");
            }

            rawArgs.push_back({argName, std::move(arg)});
        } while (ctx.match(TokenType::COMMA));
    }

    // ---------------------------
    // Arguments in multiple lines
    // ---------------------------
    ctx.consumeAllNewLines();
    if (ctx.isAtEnd())
    {
        ctx.restore(snapshot);
        throw StillParsingMultilineArgumentsSignal{};
    }

    if (!ctx.match(TokenType::RIGHT_PAREN))
    {
        throw std::runtime_error("Expected ')' to close class instantiation '" + className + "(...)'.");
    }

    std::vector<size_t> argIndices;
    auto args =
        grammar::bindArgumentsToParameters(std::move(rawArgs), paramNames, constructorScope, "Constructor", className,
                                           &defaultValues, &argIndices);

    auto constructorArgs = std::make_unique<ListExpr>(std::move(args), KnownTypes::LIST);

    auto instantiation =
        std::make_unique<CreateInstanceExpr>(className, std::move(klass), std::move(constructorArgs), std::move(argIndices));
    instantiation->validate(ctx);

    return instantiation;
}
