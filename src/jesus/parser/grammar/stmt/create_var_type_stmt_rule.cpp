#include "create_var_type_stmt_rule.hpp"
#include "../jesus_grammar.hpp"
#include "../../../ast/stmt/create_var_type_stmt.hpp"
#include "../../../types/known_types.hpp"

#include "../../../types/constraints/less_than_constraint.hpp"
#include "../../../types/constraints/greater_than_constraint.hpp"
#include "../../../types/constraints/less_than_or_equal_constraint.hpp"
#include "../../../types/constraints/greater_than_or_equal_constraint.hpp"
#include "../../../types/constraints/matches_regex_constraint.hpp"
#include "../../../interpreter/interpreter.hpp"

#include <stdexcept>

std::unique_ptr<Stmt> CreateVarTypeStmtRule::parse(ParserContext &ctx)
{
    // TODO: This code is similar to CreateVarStmtRule::parse. Unify them.

    if (!ctx.match(TokenType::CREATE))
        return nullptr;

    if (!ctx.match(TokenType::IDENTIFIER))
        throw std::runtime_error("Expected base type after 'create'");

    std::string baseTypeStr = ctx.previous().lexeme;

    if (!ctx.match(TokenType::IDENTIFIER))
        throw std::runtime_error("Expected new type name after 'create " + baseTypeStr + "'");

    std::string typeName = ctx.previous().lexeme;


    bool typeExistsLocally = ctx.varExistsInHierarchy(baseTypeStr);
    std::shared_ptr<CreationType> baseType = nullptr;

    if (typeExistsLocally ) {
        auto localType = ctx.getVarType(baseTypeStr);
        if (localType->isClass())
            baseType = localType;
    }

    if (!baseType)
    {
        baseType = KnownTypes::resolve(baseTypeStr, "core");
        if (!baseType)
            throw std::runtime_error("Unknown base type: '" + baseTypeStr + "'");
    }

    int snapshot = ctx.snapshot();
    // Here comes the key difference: we check if there’s a constraint (like > 0)
    // Otherwise it's not a variable type declaration
    if (!ctx.matchAny({TokenType::GREATER, TokenType::LESS,
                       TokenType::GREATER_EQUAL,
                       TokenType::LESS_EQUAL,
                       TokenType::MATCHES}))
    {
        return nullptr;
    }
    ctx.restore(snapshot);  // becauase matchAny above calls advance()

    std::vector<std::shared_ptr<IConstraint>> constraints;

    // Parse optional constraint(s)
    while (true)
    {
        if (ctx.match(TokenType::GREATER))
        {
            auto expr = grammar::Number->parse(ctx);
            if (!expr)
                throw std::runtime_error("Expected a number literal after '>' (e.g., '> 0')");

            Value value = ctx.interpreter->evaluate(expr);
            constraints.emplace_back(std::make_shared<GreaterThanConstraint>(value.toNumber()));
        }
        else if (ctx.match(TokenType::LESS))
        {
            auto expr = grammar::Number->parse(ctx);
            if (!expr)
                throw std::runtime_error("Expected a number literal after '<' (e.g., '< 0')");

            Value value = ctx.interpreter->evaluate(expr);
            constraints.emplace_back(std::make_shared<LessThanConstraint>(value.toNumber()));
        }
        if (ctx.match(TokenType::GREATER_EQUAL))
        {
            auto expr = grammar::Number->parse(ctx);
            if (!expr)
                throw std::runtime_error("Expected a number literal after '>=' (e.g., '>= 0')");

            Value value = ctx.interpreter->evaluate(expr);
            constraints.emplace_back(std::make_shared<GreaterThanOrEqualConstraint>(value.toNumber()));
        }
        else if (ctx.match(TokenType::LESS_EQUAL))
        {
            auto expr = grammar::Number->parse(ctx);
            if (!expr)
                throw std::runtime_error("Expected a number literal after '<=' (e.g., '<= 0')");

            Value value = ctx.interpreter->evaluate(expr);
            constraints.emplace_back(std::make_shared<LessThanOrEqualConstraint>(value.toNumber()));
        }
        else if (ctx.match(TokenType::MATCHES))
        {
            auto expr = grammar::String->parse(ctx);
            if (!expr) {
                expr = grammar::FormattedString->parse(ctx);
            }

            if (!expr)
                throw std::runtime_error("Expected a text value after 'matches' (e.g., matches \"abc\")");

            if (! expr->canEvaluateAtParseTime()) {
                throw std::runtime_error("Expected a string literal after 'matches', not a formatted string. (e.g., matches \"IForgive\")");
            }

            Value value = ctx.interpreter->evaluate(expr);
            constraints.emplace_back(std::make_shared<MatchesRegexConstraint>(value.toString()));
        }
        else
        {
            break;
        }
    }

    std::string module_name = "core"; // FIXME: consider user modules.
    return std::make_unique<CreateVarTypeStmt>(baseType, typeName, module_name, std::move(constraints));
}
