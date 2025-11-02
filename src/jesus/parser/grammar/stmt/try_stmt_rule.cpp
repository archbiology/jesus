#include "try_stmt_rule.hpp"
#include "../jesus_grammar.hpp"
#include "../../../ast/stmt/incomplete_block_stmt.hpp"
#include "../../../ast/stmt/try_stmt.hpp"

std::unique_ptr<Stmt> TryStmtRule::parse(ParserContext &ctx)
{
    if (!ctx.match(TokenType::TRY))
        return nullptr;

    if (!ctx.match(TokenType::COLON))
        throw std::runtime_error("Expected ':' after 'try' declaration.");

    std::vector<std::shared_ptr<Stmt>> tryBody;
    std::vector<std::pair<std::string, std::vector<std::shared_ptr<Stmt>>>> catchClauses;
    std::vector<std::shared_ptr<Stmt>> alwaysBody;

    ctx.consumeAllNewLines();

    // ----------------
    // Parse 'try' body
    // ----------------
    ctx.addScope(std::make_shared<Heart>("try")); // <🟢️>
    std::string blockName = "try";
    while (!ctx.checkAny({TokenType::REPENT, TokenType::ALWAYS, TokenType::AMEN}) && !ctx.isAtEnd())
    {
        tryBody.push_back(parseStmt(ctx, blockName));
        ctx.consumeAllNewLines();
    }
    ctx.popScope(); // </🟢️>

    // -----------------
    // Parse catch clauses (repent)
    // -----------------
    while (ctx.match(TokenType::REPENT))
    {
        std::string exceptionType = "ItsWritten";
        std::string varName;
        ctx.addScope(std::make_shared<Heart>(exceptionType)); // <🟢️>

        // Optional: 'repent Type'
        if (ctx.match(TokenType::IDENTIFIER))
        {
            exceptionType = ctx.previous().lexeme;

            if (ctx.match(TokenType::AS))
            {
                if (!ctx.match(TokenType::IDENTIFIER))
                    throw std::runtime_error("Expected variable name after 'as' in 'repent' clause");

                varName = ctx.previous().lexeme;
                ctx.registerVarType(exceptionType, varName);
            }
        }

        if (!ctx.match(TokenType::COLON))
            throw std::runtime_error("Expected ':' after 'repent' declaration");

        ctx.consumeAllNewLines();
        std::vector<std::shared_ptr<Stmt>> catchBody;
        blockName = exceptionType;

        while (!ctx.checkAny({TokenType::REPENT, TokenType::ALWAYS, TokenType::AMEN}) && !ctx.isAtEnd())
        {
            catchBody.push_back(parseStmt(ctx, blockName));
            ctx.consumeAllNewLines();
        }
        ctx.popScope(); // </🟢️>
        catchClauses.emplace_back(exceptionType, catchBody);
    }

    // ----------------------
    // Parse finally / always
    // ----------------------
    if (ctx.match(TokenType::ALWAYS))
    {
        if (!ctx.match(TokenType::COLON))
            throw std::runtime_error("Expected ':' after 'always'");

        blockName = "always";
        ctx.consumeAllNewLines();
        ctx.addScope(std::make_shared<Heart>(blockName)); // <🟢️>
        while (!ctx.check(TokenType::AMEN) && !ctx.isAtEnd())
        {
            alwaysBody.push_back(parseStmt(ctx, blockName));
            ctx.consumeAllNewLines();
        }
        ctx.popScope(); // </🟢️>
    }

    if (ctx.isAtEnd())
    {
        // FIXME: Each time an IncompleteBlockStmt is returned, all code is parsed again. Too expensive.
        return std::make_unique<IncompleteBlockStmt>();
    }

    if (!ctx.match(TokenType::AMEN))
        throw std::runtime_error("Expected 'amen' to close 'try' block");

    return std::make_unique<TryStmt>(tryBody, catchClauses, alwaysBody);
}

std::shared_ptr<Stmt> TryStmtRule::parseStmt(ParserContext &ctx, std::string &blockName)
{
    ctx.consumeAllNewLines();

    if (auto stmt = grammar::UpdateVar->parse(ctx))
        return stmt;

    if (auto attr = grammar::CreateVar->parse(ctx))
        return attr;

    if (auto print = grammar::Print->parse(ctx))
        return print;

    if (auto stmt = grammar::IfStmt->parse(ctx))
        return stmt;

    if (auto fight = grammar::ResistStmt->parse(ctx))
        return fight;

    throw std::runtime_error("Unexpected statement inside '" + blockName + "' body.");
}
