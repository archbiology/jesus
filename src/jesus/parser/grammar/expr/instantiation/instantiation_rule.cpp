#include "instantiation_rule.hpp"
#include "ast/expr/create_instance_expr.hpp"
#include "ast/expr/list_expr.hpp"
#include "types/known_types.hpp"

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

    // ---------------------
    // Optional constructor
    // arguments
    // ---------------------
    std::vector<std::unique_ptr<Expr>> args;

    if (!ctx.check(TokenType::RIGHT_PAREN))
    {
        do
        {
            auto arg = expression->parse(ctx);
            if (!arg)
            {
                throw std::runtime_error("Expected a constructor argument inside '" + className + "(...)'.");
            }

            args.push_back(std::move(arg));
        } while (ctx.match(TokenType::COMMA));
    }

    if (!ctx.match(TokenType::RIGHT_PAREN))
    {
        throw std::runtime_error("Expected ')' to close class instantiation '" + className + "(...)'.");
    }

    std::unique_ptr<Expr> constructorArgs = nullptr;
    if (!args.empty())
    {
        constructorArgs = std::make_unique<ListExpr>(std::move(args), KnownTypes::LIST);
    }

    return std::make_unique<CreateInstanceExpr>(className, std::move(klass), std::move(constructorArgs));
}
