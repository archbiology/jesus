#include "parser.hpp"
#include "grammar/jesus_grammar.hpp"
#include "parser_context.hpp"
#include "../ast/stmt/repeat_times_stmt.hpp"
#include "../ast/stmt/print_stmt.hpp"
#include <bits/stdc++.h> // for std::find_if and std::distance
#include <string>
#include <memory>

std::unique_ptr<Stmt> parse(const std::vector<Token> &tokens, ParserContext &context)
{
    const int tokens_count = tokens.size();
    if (!tokens_count)
        return nullptr;

    context.consumeAllNewLines();
    if (context.isAtEnd())
        return nullptr;

    int snapshot = context.snapshot();
    auto outputStmt = grammar::Print->parse(context);
    if (outputStmt)
        return outputStmt;

    context.restore(snapshot);
    auto createClassStmt = grammar::CreateClass->parse(context);
    if (createClassStmt)
        return createClassStmt;

    context.restore(snapshot);
    auto createVarTypeStmt = grammar::CreateVarType->parse(context);
    if (createVarTypeStmt)
        return createVarTypeStmt;

    context.restore(snapshot);
    auto createVarStmt = grammar::CreateVar->parse(context);
    if (createVarStmt)
        return createVarStmt;

    context.restore(snapshot);
    auto updateVarStmt = grammar::UpdateVar->parse(context);
    if (updateVarStmt)
        return updateVarStmt;

    auto repeatWhileStmt = grammar::RepeatWhile->parse(context);
    if (repeatWhileStmt)
        return repeatWhileStmt;

    context.restore(snapshot);
    auto ifStmt = grammar::IfStmt->parse(context);
    if (ifStmt)
        return ifStmt;

    context.restore(snapshot);
    auto tryStmt = grammar::TryStmt->parse(context);
    if (tryStmt)
        return tryStmt;

    context.restore(snapshot);
    auto resistStmt = grammar::ResistStmt->parse(context);
    if (resistStmt)
        return resistStmt;

    context.restore(snapshot);
    auto importModuleStmt = grammar::ImportModuleStmt->parse(context);
    if (importModuleStmt)
        return importModuleStmt;

    context.restore(snapshot);
    auto astInspectStmt = grammar::AstInspectStmt->parse(context);
    if (astInspectStmt)
        return astInspectStmt;

    // If no match, fall back to expression parsing
    context.restore(snapshot);
    auto expr = grammar::Expression->parse(context);
    if (!expr)
    {
        throw std::runtime_error("Unknown expression type (parser.cpp)");
    }

    // Using WARN because the person didn't call 'say'
    return std::make_unique<PrintStmt>(StmtType::WARN, std::move(expr));
}
