#include "create_method_stmt_rule.hpp"
#include "../../../ast/stmt/create_method_stmt.hpp"
#include "../../../ast/stmt/incomplete_block_stmt.hpp"
#include "../../parser_context.hpp"
#include <stdexcept>

std::unique_ptr<Stmt> CreateMethodStmtRule::parse(ParserContext &ctx)
{
    // ----------------------------------------
    // Grammar:
    //  purpose <name> ( <params>? ) : ... amen
    // ----------------------------------------

    if (!ctx.match(TokenType::PURPOSE))
        return nullptr;

    if (!ctx.match(TokenType::IDENTIFIER))
        throw std::runtime_error("Expected method name after 'calling'");

    std::string methodName = ctx.previous().lexeme;

    // ----------
    // Parameters
    // ----------
    if (!ctx.match(TokenType::LEFT_PAREN))
        throw std::runtime_error("Expected '(' after method name in 'calling' statement.");

    Heart params;

    if (!ctx.check(TokenType::RIGHT_PAREN))
    {
        do
        {
            if (!ctx.match(TokenType::IDENTIFIER))
                throw std::runtime_error("Expected parameter type in method declaration.");

            std::string type = ctx.previous().lexeme;

            if (!ctx.match(TokenType::IDENTIFIER))
                throw std::runtime_error("Expected parameter name after type '" + type + "'.");

            std::string name = ctx.previous().lexeme;

            params.createVar(name, Value(1)); // FIXME: Validate `type` and allow initial values

        } while (ctx.match(TokenType::SEMICOLON)); // TODO: allow more args of same type: int x, y, z; string name, surname;
    }

    if (!ctx.match(TokenType::RIGHT_PAREN))
        throw std::runtime_error("Expected ')' after parameter list in method declaration.");

    // -----------
    // Method body
    // -----------
    if (!ctx.match(TokenType::COLON))
        throw std::runtime_error("Expected ':' after method signature.");

    std::vector<std::shared_ptr<Stmt>> body;

    while (!ctx.check(TokenType::AMEN) && !ctx.isAtEnd())
    {
        if (auto stmt = updateVar->parse(ctx))
        {
            body.push_back(std::move(stmt));
        }
        else if (auto attr = createVar->parse(ctx))
        {
            body.push_back(std::move(attr));
        }
        else
        {
            throw std::runtime_error("Unexpected statement inside method body.");
        }
    }

    if (ctx.isAtEnd())
    {
        return std::make_unique<IncompleteBlockStmt>();
    }

    if (!ctx.match(TokenType::AMEN))
        throw std::runtime_error("Expected 'amen' to close method body.");

    return std::make_unique<CreateMethodStmt>(methodName, params, body, /*isGenesis=*/false);
}
