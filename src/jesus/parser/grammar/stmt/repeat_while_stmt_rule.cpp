#include "repeat_while_stmt_rule.hpp"
#include "update_var_stmt_rule.hpp"
#include "../../../ast/stmt/update_var_stmt.hpp"
#include "../../../ast/stmt/update_var_with_ask_stmt.hpp"
#include "../../../ast/stmt/repeat_while_stmt.hpp"
#include "../../../ast/stmt/incomplete_block_stmt.hpp"
#include "../jesus_grammar.hpp"
#include <stdexcept>

std::unique_ptr<Stmt> RepeatWhileStmtRule::parse(ParserContext &ctx)
{
    // --------------------------------
    // Step 1: Must start with 'repeat'
    // --------------------------------
    if (!ctx.match(TokenType::REPEAT))
        return nullptr;

    // -------------------------
    // Step 2: Must have 'while'
    // -------------------------
    if (!ctx.match(TokenType::WHILE))
        throw std::runtime_error("Expected 'while' after 'repeat'.");

    // --------------------------------------
    // Step 3: Parse the condition expression
    // --------------------------------------
    auto condition = grammar::Expression->parse(ctx);
    if (!condition)
        throw std::runtime_error("Expected a condition expression after 'repeat while'.");

    // --------------------
    // Step 4: Expect colon
    // --------------------
    if (!ctx.match(TokenType::COLON))
        throw std::runtime_error("Expected ':' after repeat while condition.");

    // ------------------------------------------
    // Step 5: Parse body statements until 'amen'
    // ------------------------------------------
    std::vector<std::unique_ptr<Stmt>> body;
    ctx.consumeAllNewLines();

    while (!ctx.check(TokenType::AMEN) && !ctx.isAtEnd())
    {
        // Reuse existing statements parsers
        if (auto stmt = grammar::Print->parse(ctx))
        {
            body.push_back(std::move(stmt));
        }
        else if (auto stmt = grammar::CreateVar->parse(ctx))
        {
            body.push_back(std::move(stmt));
        }
        else if (auto stmt = grammar::UpdateVar->parse(ctx))
        {
            body.push_back(std::move(stmt));
        }
        else
        {
            throw std::runtime_error("Unexpected statement inside repeat-while block.");
        }

        ctx.consumeAllNewLines();
    }

    if (ctx.isAtEnd())
    {
        // FIXME: Each time an IncompleteBlockStmt is returned, all code is parsed again. Too expensive.
        return std::make_unique<IncompleteBlockStmt>();
    }

    if (!ctx.match(TokenType::AMEN))
        throw std::runtime_error("Expected 'amen' to close repeat-while block.");

    return std::make_unique<RepeatWhileStmt>(std::move(condition), std::move(body));
}
