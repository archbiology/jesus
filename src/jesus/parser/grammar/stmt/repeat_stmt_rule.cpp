#include "repeat_stmt_rule.hpp"
#include "update_var_stmt_rule.hpp"
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

std::unique_ptr<Stmt> RepeatStmtRule::parse(ParserContext &ctx)
{
    // --------------------------------
    // Step 1: Must start with 'repeat'
    // --------------------------------
    if (!ctx.match(TokenType::REPEAT))
        return nullptr;

    std::vector<std::unique_ptr<Stmt>> body;
    // ---------------------------
    // Case A: repeat while <expr>
    // ---------------------------
    if (ctx.match(TokenType::WHILE))
    {
        auto condition = grammar::Expression->parse(ctx);
        if (!condition)
            throw std::runtime_error("Expected a condition expression after 'repeat while'.");

        consumeColon(ctx);
        try
        {
            body = parseBody(ctx);
        }
        catch (const IncompleteBlockStmtSignal &)
        {
            return std::make_unique<IncompleteBlockStmt>();
        }

        return std::make_unique<RepeatWhileStmt>(std::move(condition), std::move(body));
    }

    // ----------------------
    // Case B: repeat forever
    // ----------------------
    if (ctx.match(TokenType::FOREVER))
    {
        consumeColon(ctx);
        try
        {
            body = parseBody(ctx);
        }
        catch (const IncompleteBlockStmtSignal &)
        {
            return std::make_unique<IncompleteBlockStmt>();
        }

        return std::make_unique<RepeatForeverStmt>(std::move(body));
    }

    // ----------------------
    // Case C: repeat N times
    // ----------------------
    std::unique_ptr<Expr> countExpr;
    if (ctx.check(TokenType::INT))
    {
        countExpr = grammar::Number->parse(ctx);
    }
    else if (ctx.check(TokenType::IDENTIFIER))
    {
        // allow variable or attribute reference
        countExpr = grammar::GetAttribute->parse(ctx);

        if (!countExpr->canBeUsedInRepeatTimesStmt())
        {
            throw std::runtime_error(
                "NasaRules: Only simple variables or attribute are allowed for 'repeat x times'.");
        }

        auto varName = ctx.previous();
        auto varType = countExpr->getReturnType(ctx);
        if (!varType->isNumber())
            throw std::runtime_error("NasaRule2: repeat times expects an integer variable or attribute, but '" + varName.lexeme + "' is of type " + varType->name);
    }
    else
    {
        throw std::runtime_error("Expected 'while', 'forever', a number, or variable after 'repeat'.");
    }

    if (!ctx.match(TokenType::TIMES))
        throw std::runtime_error("Expected 'times' after repeat count.");

    consumeColon(ctx);
    try
    {
        body = parseBody(ctx);
    }
    catch (const IncompleteBlockStmtSignal &)
    {
        return std::make_unique<IncompleteBlockStmt>();
    }

    return std::make_unique<RepeatTimesStmt>(std::move(countExpr), std::move(body));
}

void RepeatStmtRule::consumeColon(ParserContext &ctx)
{
    if (!ctx.match(TokenType::COLON))
        throw std::runtime_error("Expected ':' after repeat statement.");
}

std::vector<std::unique_ptr<Stmt>> RepeatStmtRule::parseBody(ParserContext &ctx)
{
    std::vector<std::unique_ptr<Stmt>> body;
    ctx.consumeAllNewLines();

    while (!ctx.check(TokenType::AMEN) && !ctx.isAtEnd())
    {
        if (auto stmt = grammar::Print->parse(ctx))
            body.push_back(std::move(stmt));
        else if (auto stmt = grammar::CreateVar->parse(ctx))
            body.push_back(std::move(stmt));
        else if (auto stmt = grammar::UpdateVar->parse(ctx))
            body.push_back(std::move(stmt));
        else if (ctx.match(TokenType::SKIP))
            body.push_back(std::make_unique<SkipStmt>());
        else if (ctx.match(TokenType::BREAK))
            body.push_back(std::make_unique<BreakStmt>());
        else
            throw std::runtime_error("Unexpected statement inside repeat block.");

        ctx.consumeAllNewLines();
    }

    if (ctx.isAtEnd())
    {
        // FIXME: Each time an IncompleteBlockStmt is returned, all code is parsed again. Too expensive.
        // Why not just keep from where it stopped, remembering what was already in the `body` vector?
        throw IncompleteBlockStmtSignal{};
    }

    if (!ctx.match(TokenType::AMEN))
        throw std::runtime_error("Expected 'amen' to close repeat block.");

    return body;
}
