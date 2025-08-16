#include "create_class_stmt_rule.hpp"
#include "../../../ast/stmt/create_class_stmt.hpp"
#include "../../../ast/stmt/incomplete_block_stmt.hpp"
#include <stdexcept>

std::unique_ptr<Stmt> CreateClassStmtRule::parse(ParserContext &ctx)
{
    std::string stmt = "let there be";

    if (ctx.match(TokenType::HAJA))
    {
        stmt = "haja"; // haja Luz: amém
    }
    else
    {
        if (!ctx.match(TokenType::LET))
            return nullptr;

        if (!ctx.match(TokenType::THERE))
            return nullptr;

        if (!ctx.match(TokenType::BE))
            throw std::runtime_error("Expected 'be' after 'let there'");
    }

    if (!ctx.match(TokenType::IDENTIFIER))
        throw std::runtime_error("Expected class name after '" + stmt + "'");

    std::string className = ctx.previous().lexeme;

    if (!ctx.match(TokenType::COLON))
        throw std::runtime_error("Expected ':' after class name in '" + stmt + "' statement.");

    // (MVP: no parsing body for the moment; only spirit for now)
    std::vector<std::shared_ptr<Stmt>> body;

    if (ctx.isAtEnd())
    {
        return std::make_unique<IncompleteBlockStmt>();
    }

    if (!ctx.match(TokenType::AMEN))
        throw std::runtime_error("Expected 'amen' after ':' in '" + stmt + "' statement.");

    ctx.registerClassName(className);

    std::string module_name = "core"; // FIXME: consider user modules.
    return std::make_unique<CreateClassStmt>(className, module_name, body);
}
