#include "create_class_stmt_rule.hpp"
#include "../../../ast/stmt/create_class_stmt.hpp"
#include <stdexcept>

std::unique_ptr<Stmt> CreateClassStmtRule::parse(ParserContext &ctx)
{
    if (!ctx.match(TokenType::CREATION))
        return nullptr;

    if (!ctx.match(TokenType::IDENTIFIER))
        throw std::runtime_error("Expected class name after 'creation'");

    std::string className = ctx.previous().lexeme;

    if (!ctx.match(TokenType::COLON))
        throw std::runtime_error("Expected ':' after class name in creation statement.");

    // (MVP: no parsing body for the moment; only spirit for now)
    std::vector<std::shared_ptr<Stmt>> body;

    if (!ctx.match(TokenType::AMEN))
        throw std::runtime_error("Expected 'amen' after ':' in creation statement.");

    ctx.registerClassName(className);

    std::string module_name = "core"; // FIXME: consider user modules.
    return std::make_unique<CreateClassStmt>(className, module_name, body);
}
