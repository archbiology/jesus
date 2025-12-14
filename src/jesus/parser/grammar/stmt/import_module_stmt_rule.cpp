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
    ctx.match(TokenType::COME);

    auto path = parseModulePath(ctx);

    return std::make_unique<ImportModuleStmt>(path.depth, path.parts, "");
}

std::unique_ptr<Stmt> ImportModuleStmtRule::parseFromModuleComeMember(ParserContext &ctx)
{
    ctx.match(TokenType::FROM);

    auto path = parseModulePath(ctx);

    if (!ctx.match(TokenType::COME))
        throw std::runtime_error("Expected 'come' after module name.");

    if (!ctx.match(TokenType::IDENTIFIER))
        throw std::runtime_error("Expected import name after 'come'.");

    std::string alias = ctx.previous().lexeme;
    return std::make_unique<ImportModuleStmt>(path.depth, path.parts, alias);
}

ParsedModulePath ImportModuleStmtRule::parseModulePath(ParserContext &ctx)
{
    ParsedModulePath result{0, {}};

    // optionally parse relative prefix (.. ... ....)
    if (ctx.check(TokenType::DOTS))
    {
        auto token = ctx.advance();
        result.depth = token.lexeme.size() - 1; // ".." → depth 1, "..." → 2
    }

    // parse one or more identifiers
    if (!ctx.match(TokenType::IDENTIFIER))
        throw std::runtime_error("Expected module name.");

    result.parts.push_back(ctx.previous().lexeme);

    while (ctx.match(TokenType::IDENTIFIER))
    {
        result.parts.push_back(ctx.previous().lexeme);
    }

    return result;
}