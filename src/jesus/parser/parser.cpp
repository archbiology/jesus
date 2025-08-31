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

    // repeat n times
    if (tokens_count >= 4 &&
        tokens[0].type == TokenType::REPEAT &&
        tokens[1].type == TokenType::INT &&
        tokens[2].type == TokenType::TIMES &&
        tokens[3].lexeme == ":")
    {
        int repeatCount = std::stoi(tokens[1].lexeme); // assuming it's an integer literal

        std::vector<Token> innerTokens(tokens.begin() + 4, tokens.end());

        std::unique_ptr<Stmt> innerStmt = parse(innerTokens, context);
        if (!innerStmt)
        {
            throw std::runtime_error("Could not parse body of 'repeat N times'");
        }

        return std::make_unique<RepeatTimesStmt>(repeatCount, std::move(innerStmt));
    }

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

    auto updateVarStmt = grammar::UpdateVar->parse(context);
    if (updateVarStmt)
        return updateVarStmt;

    // If no match, fall back to expression parsing
    auto expr = grammar::Expression->parse(context);
    if (!expr)
    {
        throw std::runtime_error("Unknown expression type (parser.cpp)");
    }

    // Using WARN because the person didn't call 'say'
    return std::make_unique<PrintStmt>(StmtType::WARN, std::move(expr));
}
