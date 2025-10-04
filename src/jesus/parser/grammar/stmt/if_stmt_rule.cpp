#include "if_stmt_rule.hpp"
#include "../../../ast/stmt/if_stmt.hpp"
#include "../../../ast/stmt/update_var_stmt.hpp"
#include "../../../ast/stmt/update_var_with_ask_stmt.hpp"
#include "../../../ast/stmt/repeat_while_stmt.hpp"
#include "../../../ast/stmt/repeat_times_stmt.hpp"
#include "../../../ast/stmt/repeat_forever_stmt.hpp"
#include "../../../ast/stmt/skip_stmt.hpp"
#include "../../../ast/stmt/break_stmt.hpp"
#include "../../../ast/stmt/incomplete_block_stmt.hpp"
#include "../jesus_grammar.hpp"
#include <stdexcept>

class IncompleteBlockStmtSignal : public std::exception
{
public:
    const char *what() const noexcept override
    {
        return "Incomplete block detected (waiting for more input)";
    }
};

std::unique_ptr<Stmt> IfStmtRule::parse(ParserContext &ctx)
{
    // -----------------------------------------
    // Grammar:
    //  if <condition> : ... amen
    //  if <condition> : ... otherwise: ... amen
    // -----------------------------------------

    if (!ctx.match(TokenType::IF))
        return nullptr;

    auto condition = grammar::Expression->parse(ctx);
    if (!condition)
        throw std::runtime_error("Expected expression after 'if' statement.");

    if (!ctx.match(TokenType::COLON))
        throw std::runtime_error("Expected ':' after if condition.");

    std::vector<std::unique_ptr<Stmt>> thenBranch;
    std::vector<std::unique_ptr<Stmt>> otherwiseBranch;

    ctx.consumeAllNewLines();

    // -------------------
    // Parse the 'if' body
    // -------------------
    while (!ctx.check(TokenType::OTHERWISE) && !ctx.check(TokenType::AMEN) && !ctx.isAtEnd())
    {
        if (auto stmt = grammar::Print->parse(ctx))
            thenBranch.push_back(std::move(stmt));
        else if (auto stmt = grammar::CreateVar->parse(ctx))
            thenBranch.push_back(std::move(stmt));
        else if (auto stmt = grammar::UpdateVar->parse(ctx))
            thenBranch.push_back(std::move(stmt));
        else if (auto stmt = grammar::RepeatWhile->parse(ctx))
            thenBranch.push_back(std::move(stmt));
        else if (ctx.match(TokenType::SKIP))
            thenBranch.push_back(std::make_unique<SkipStmt>());
        else if (ctx.match(TokenType::BREAK))
            thenBranch.push_back(std::make_unique<BreakStmt>());
        else
            throw std::runtime_error("Unexpected statement inside 'if' body.");

        ctx.consumeAllNewLines();
    }

    // ------------------
    // Parse 'otherwise:'
    // ------------------
    if (ctx.match(TokenType::OTHERWISE))
    {
        if (!ctx.match(TokenType::COLON))
            throw std::runtime_error("Expected ':' after 'otherwise'.");

        ctx.consumeAllNewLines();

        while (!ctx.check(TokenType::AMEN) && !ctx.isAtEnd())
        {
            if (auto stmt = grammar::Print->parse(ctx))
                otherwiseBranch.push_back(std::move(stmt));
            else if (auto stmt = grammar::CreateVar->parse(ctx))
                otherwiseBranch.push_back(std::move(stmt));
            else if (auto stmt = grammar::UpdateVar->parse(ctx))
                otherwiseBranch.push_back(std::move(stmt));
            else if (auto stmt = grammar::RepeatWhile->parse(ctx))
                otherwiseBranch.push_back(std::move(stmt));
            else if (ctx.match(TokenType::SKIP))
                otherwiseBranch.push_back(std::make_unique<SkipStmt>());
            else if (ctx.match(TokenType::BREAK))
                otherwiseBranch.push_back(std::make_unique<BreakStmt>());

            else
                throw std::runtime_error("Unexpected statement inside 'otherwise' body.");

            ctx.consumeAllNewLines();
        }
    }

    if (ctx.isAtEnd())
    {
        // FIXME: Each time an IncompleteBlockStmt is returned, all code is parsed again. Too expensive.
        return std::make_unique<IncompleteBlockStmt>();
    }

    if (!ctx.match(TokenType::AMEN))
        throw std::runtime_error("Expected 'amen' to close 'if' statement.");

    return std::make_unique<IfStmt>(
        std::move(condition),
        std::move(thenBranch),
        std::move(otherwiseBranch));
}
