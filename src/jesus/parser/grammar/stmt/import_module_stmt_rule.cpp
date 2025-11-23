#include "import_module_stmt_rule.hpp"
#include "../../../ast/stmt/import_module_stmt.hpp"
#include <stdexcept>

std::unique_ptr<Stmt> ImportModuleStmtRule::parse(ParserContext &ctx)
{

    if (ctx.check(TokenType::COME))
        return parseComeModule(ctx);

    if (ctx.check(TokenType::FROM))
        return parseFromModuleComeMember(ctx);

    return nullptr;
}

std::unique_ptr<Stmt> ImportModuleStmtRule::parseComeModule(ParserContext &ctx)
{
    if (!ctx.match(TokenType::COME))
        return nullptr;

    if (!ctx.match(TokenType::IDENTIFIER))
        throw std::runtime_error("Expected module name after 'come'.");

    std::string module = ctx.previous().lexeme;
    return std::make_unique<ImportModuleStmt>(module, "");
}

std::unique_ptr<Stmt> ImportModuleStmtRule::parseFromModuleComeMember(ParserContext &ctx)
{
    if (!ctx.match(TokenType::FROM))
        return nullptr;

    if (!ctx.match(TokenType::IDENTIFIER))
        throw std::runtime_error("Expected module name after 'from'.");

    std::string module = ctx.previous().lexeme;

    if (!ctx.match(TokenType::COME))
        throw std::runtime_error("Expected 'come' after module name.");

    if (!ctx.match(TokenType::IDENTIFIER))
        throw std::runtime_error("Expected import name after 'come'.");

    std::string alias = ctx.previous().lexeme;
    return std::make_unique<ImportModuleStmt>(module, alias);
}
