#include "foreach_stmt_rule.hpp"
#include "update_var_stmt_rule.hpp"
#include "ast/stmt/update_var_stmt.hpp"
#include "ast/stmt/update_var_with_ask_stmt.hpp"
#include "ast/stmt/for_each_stmt.hpp"
#include "ast/stmt/skip_stmt.hpp"
#include "ast/stmt/break_stmt.hpp"
#include "ast/stmt/incomplete_block_stmt.hpp"
#include "parser/grammar/jesus_grammar.hpp"
#include "types/known_types.hpp"
#include "types/composite/list_type.hpp"
#include <stdexcept>

class IncompleteBlockStmtSignal : public std::exception
{
public:
    const char *what() const noexcept override
    {
        return "Incomplete block detected (waiting for more input)";
    }
};

std::unique_ptr<Stmt> ForEachStmtRule::parse(ParserContext &ctx)
{
    // --------------------------------
    // Step 1: Must start with 'foreach'
    // --------------------------------
    if (!ctx.match(TokenType::FOREACH))
        return nullptr;

    if (!ctx.match(TokenType::IDENTIFIER))
        throw std::runtime_error("Expected variable name after 'foreach'. ");

    std::vector<std::string> varNames;
    varNames.push_back(ctx.previous().lexeme);

    while (ctx.match(TokenType::COMMA))
    {
        if (!ctx.match(TokenType::IDENTIFIER))
        {
            throw std::runtime_error("Expected variable name after ','.");
        }

        varNames.push_back(ctx.previous().lexeme);
    }

    if (varNames.size() > 2)
        throw std::runtime_error("foreach supports at most two variables.");

    if (!ctx.match(TokenType::IN))
        throw std::runtime_error("Expected 'in' after variable name.");

    auto iterable = grammar::Expression->parse(ctx);
    if (!iterable)
    {
        throw std::runtime_error("Expected iterable expression after 'in'.");
    }

    if (!ctx.match(TokenType::COLON))
        throw std::runtime_error("Expected ':' after foreach statement.");

    auto iterableType = iterable->getReturnType(ctx);
    if (!iterableType->isA(KnownTypes::LIST))
        throw std::runtime_error("Expected a list after 'foreach key in'. Got '" + iterableType->name + "' instead.");

    auto listType = std::dynamic_pointer_cast<ListType>(iterableType);

    auto elementType = listType->elementType;

    auto foreachScope = std::make_shared<Heart>("foreach." + varNames[0]);
    if (varNames.size() == 1)
    {
        foreachScope->createVar(elementType, varNames[0], Value::formless());
    }
    else
    {
        auto innerListType = std::dynamic_pointer_cast<ListType>(listType->elementType);

        if (!innerListType)
        {
            throw std::runtime_error(
                "foreach key,value requires a list of pairs. "
                "The iterable element type is '" +
                listType->elementType->name + "'.");
        }

        foreachScope->createVar(innerListType->elementType, varNames[0], Value::formless());
        foreachScope->createVar(innerListType->elementType, varNames[1], Value::formless());
    }

    // ----------
    // Parse body
    // ----------
    ctx.addScope(foreachScope); // <🟢️>
    std::vector<std::unique_ptr<Stmt>> body;
    try
    {
        body = parseBody(ctx);
    }
    catch (const IncompleteBlockStmtSignal &)
    {
        return std::make_unique<IncompleteBlockStmt>();
    }
    ctx.popScope(); // </🟢️>

    return std::make_unique<ForEachStmt>(std::move(varNames), std::move(iterable), std::move(body));
}

std::vector<std::unique_ptr<Stmt>> ForEachStmtRule::parseBody(ParserContext &ctx)
{
    // FIXME: This code is repeated inside RepeatStmtRule
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
        else if (auto stmt = grammar::IfStmt->parse(ctx))
            body.push_back(std::move(stmt));
        else if (ctx.match(TokenType::SKIP))
            body.push_back(std::make_unique<SkipStmt>());
        else if (ctx.match(TokenType::BREAK))
            body.push_back(std::make_unique<BreakStmt>());
        else
            throw std::runtime_error("Unexpected statement inside foreach block.");

        ctx.consumeAllNewLines();
    }

    if (ctx.isAtEnd())
    {
        // FIXME: Each time an IncompleteBlockStmt is returned, all code is parsed again. Too expensive.
        // Why not just keep from where it stopped, remembering what was already in the `body` vector?
        throw IncompleteBlockStmtSignal{};
    }

    if (!ctx.match(TokenType::AMEN))
        throw std::runtime_error("Expected 'amen' to close foreach block.");

    return body;
}
