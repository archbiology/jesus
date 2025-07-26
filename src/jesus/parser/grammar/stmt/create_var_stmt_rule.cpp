#include "create_var_stmt_rule.hpp"
#include "../../../ast/stmt/create_var_stmt.hpp"
#include "../../../types/known_types.hpp"
#include "../../../interpreter/interpreter.hpp"
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

    const auto *creationType = KnownTypes::resolve(varType, "core");
    if (!creationType)
    {
        throw std::runtime_error("Unknown variable type: '" + varType + "'");
    }

    std::unique_ptr<Expr> value = nullptr;

    if (ctx.match(TokenType::EQUAL))
    {
        value = expression->parse(ctx);
        if (!value)
            throw std::runtime_error("Expected expression after '=' in create statement.");


        // Evaluating functions that could send an 'ok' to bomb explosions is not safe.
        // Let's warn this until we can check the return type from functions
        std::cerr << "🔴️ FIXME: Should not call interpreter here. Remove this once functions and static type inference are implemented.\n";

        Value evaluated = ctx.interpreter->evaluate(value);
        if (!creationType->validate(evaluated))
        {
            throw std::runtime_error("Error: Invalid value value \"" + evaluated.toString() + "\" to variable '"+ varName +"' declared as type " + creationType->name);
        }

        ctx.registerVarType(varName, creationType->name);
    }

    return std::make_unique<CreateVarStmt>(varType, varName, std::move(value));
}
