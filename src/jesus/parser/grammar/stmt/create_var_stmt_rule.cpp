#include "create_var_stmt_rule.hpp"
#include "../../../ast/stmt/create_var_stmt.hpp"
#include "../../../ast/stmt/create_var_with_ask_stmt.hpp"
#include "../../../types/known_types.hpp"
#include "../../../interpreter/interpreter.hpp"
#include "../../../interpreter/runtime/instance.hpp"
#include <stdexcept>

std::unique_ptr<Stmt> CreateVarStmtRule::parse(ParserContext &ctx)
{
    if (!ctx.match(TokenType::CREATE))
        return nullptr;

    if (!ctx.match(TokenType::IDENTIFIER))
        throw std::runtime_error("Expected variable type after 'create'");

    std::string varType = ctx.previous().lexeme;

    if (!ctx.match(TokenType::IDENTIFIER))
        throw std::runtime_error("Expected variable name after 'create type'");

    std::string varName = ctx.previous().lexeme;

    auto creationType = KnownTypes::resolve(varType, "core");
    if (!creationType)
    {
        throw std::runtime_error("Unknown variable type: '" + varType + "'");
    }

    std::unique_ptr<Expr> value = nullptr;

    if (ctx.match(TokenType::EQUAL))
    {
        int snapshot = ctx.snapshot();
        if (ctx.match(TokenType::ASK))
        {
            ctx.restore(snapshot); // because 'ask->parse()' expects TokenType::ASK, but since it matched, it already advanced.

            std::unique_ptr<Expr> ask_expr = ask->parse(ctx);
            if (!ask_expr)
                throw std::runtime_error("Expected a text literal or a text-typed variable after 'ask' (e.g., ask \"What is your name?\" or ask question).");

            ctx.registerVarType(creationType->name, varName);

            return std::make_unique<CreateVarWithAskStmt>(*creationType, varName, std::move(ask_expr));
        }
        value = expression->parse(ctx);
        if (!value)
            throw std::runtime_error("Expected expression after '=' in create statement.");

        // Evaluating functions that could send an 'ok' to bomb explosions is not safe.
        // Let's warn this until we can check the return type from functions
        std::cerr << "🔴️ FIXME: Should not call interpreter here. Remove this once functions and static type inference are implemented.\n";

        Value evaluated = ctx.interpreter->evaluate(value);
        if (!creationType->validate(evaluated))
        {
            throw std::runtime_error("Error: Invalid value value \"" + evaluated.toString() + "\" to variable '" + varName + "' declared as type " + creationType->name);
        }

        if (creationType->isClass())
        {
            auto instance = std::make_shared<Instance>(creationType);
            evaluated = Value(instance);
            value = std::make_unique<LiteralExpr>(evaluated); // TODO: Consider constructor and 'if' expression
        }

        ctx.registerVarType(creationType->name, varName);
    }

    return std::make_unique<CreateVarStmt>(varType, varName, std::move(value));
}
