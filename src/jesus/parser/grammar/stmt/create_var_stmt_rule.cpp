#include "create_var_stmt_rule.hpp"
#include "../../../ast/stmt/create_var_stmt.hpp"
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

    std::unique_ptr<Expr> value = nullptr;

    if (ctx.match(TokenType::EQUAL))
    {
        value = expression->parse(ctx);
        if (!value)
            throw std::runtime_error("Expected expression after '=' in create statement.");
    }

    return std::make_unique<CreateVarStmt>(varType, varName, std::move(value));
}
