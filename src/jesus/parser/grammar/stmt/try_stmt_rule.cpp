#include "try_stmt_rule.hpp"
#include "../jesus_grammar.hpp"
#include "ast/stmt/incomplete_block_stmt.hpp"
#include "ast/stmt/try_stmt.hpp"
#include "types/known_types.hpp"
#include "lexer/keywords.hpp"

std::unique_ptr<Stmt> TryStmtRule::parse(ParserContext &ctx)
{
    if (!ctx.match(TokenType::TRY))
        return nullptr;

    if (!ctx.match(TokenType::COLON))
        throw std::runtime_error("Expected ':' after 'try' declaration.");

    std::vector<std::unique_ptr<Stmt>> tryBody;
    std::vector<std::pair<std::string, std::vector<std::unique_ptr<Stmt>>>> catchClauses;
    std::vector<std::unique_ptr<Stmt>> alwaysBody;

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
        auto ItsWritten = KnownTypes::EXCEPTION;
        auto exceptionType = ItsWritten;
        std::string exceptionTypeStr = exceptionType->name;
        std::string varName;
        ctx.addScope(std::make_shared<Heart>(exceptionTypeStr)); // <🟢️>


        // Optional: 'repent Type'
        if (ctx.match(TokenType::IDENTIFIER))
        {
            exceptionTypeStr = ctx.previous().lexeme;

            if (ctx.match(TokenType::AS))
            {
                if (ctx.check(TokenType::IDENTIFIER))
                {
                    ctx.advance();
                }
                else if (Keywords::isReservedWord(ctx.peek().lexeme))
                {
                    std::string varName = ctx.peek().lexeme;
                    throw std::runtime_error(Keywords::reservedWordMsg(varName, "variable"));
                }
                else
                {
                    throw std::runtime_error("Expected variable name after 'as' in 'repent' clause");
                }

                varName = ctx.previous().lexeme;
                exceptionType = KnownTypes::resolve(exceptionTypeStr, "core"); // FIXME: not always 'core'. User define their own exceptions.
                ctx.registerVarType(exceptionType, varName);
            }
        }

        if (!ctx.match(TokenType::COLON))
            throw std::runtime_error("Expected ':' after 'repent' declaration");

        ctx.consumeAllNewLines();
        std::vector<std::unique_ptr<Stmt>> catchBody;
        blockName = exceptionType->name;

        while (!ctx.checkAny({TokenType::REPENT, TokenType::ALWAYS, TokenType::AMEN}) && !ctx.isAtEnd())
        {
            catchBody.push_back(parseStmt(ctx, blockName));
            ctx.consumeAllNewLines();
        }
        ctx.popScope(); // </🟢️>
        catchClauses.emplace_back(exceptionTypeStr, std::move(catchBody));
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

    return std::make_unique<TryStmt>(std::move(tryBody), std::move(catchClauses), std::move(alwaysBody));
}

std::unique_ptr<Stmt> TryStmtRule::parseStmt(ParserContext &ctx, std::string &blockName)
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
