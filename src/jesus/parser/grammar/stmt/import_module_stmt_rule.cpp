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

    std::vector<ImportItem> importedSymbols;
    return std::make_unique<ImportModuleStmt>(path.depth, path.parts, importedSymbols);
}

std::unique_ptr<Stmt> ImportModuleStmtRule::parseFromModuleComeMember(ParserContext &ctx)
{
    ctx.match(TokenType::FROM);

    auto path = parseModulePath(ctx);

    if (!ctx.match(TokenType::COME))
        throw std::runtime_error("Expected 'come' after module name.");

    std::vector<ImportItem> importedSymbols;
    importedSymbols.push_back(parseImportItem(ctx));

    while (ctx.match(TokenType::COMMA))
    {
        if (!ctx.match(TokenType::IDENTIFIER))
            throw std::runtime_error("Expected symbol after ','");

        importedSymbols.push_back(parseImportItem(ctx));
    }

    return std::make_unique<ImportModuleStmt>(path.depth, path.parts, importedSymbols);
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
        throw std::runtime_error("Expected import name after 'come'.");

    result.parts.push_back(ctx.previous().lexeme);

    while (ctx.match(TokenType::IDENTIFIER))
    {
        result.parts.push_back(ctx.previous().lexeme);
    }

    return result;
}

ImportItem ImportModuleStmtRule::parseImportItem(ParserContext &ctx)
{
    if (!ctx.match(TokenType::IDENTIFIER))
        throw std::runtime_error("Expected symbol name.");

    ImportItem item;
    item.originalName = ctx.previous().lexeme;

    if (ctx.match(TokenType::AS))
    {
        if (!ctx.match(TokenType::IDENTIFIER))
            throw std::runtime_error("Expected alias after 'as'.");

        item.alias = ctx.previous().lexeme;
    }

    return item;
}
